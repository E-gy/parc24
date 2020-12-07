#include <parc24/iosstack.h>

#include <util/null.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct iop* StackOp;
struct iop {
	enum {
		BOOKMARK = 0,
		IOP_OPEN,
		IOP_CLOSE,
		IOP_COPY,
		IOP_DUP,
	} type;
	union {
		struct {
			fd_t stream;
			fd_t cl_rio;
		} open;
		struct {
			fd_t stream;
			fd_t cl_rio;
		} close;
		struct {
			fd_t dstream;
			fd_t sstream;
			fd_t cl_rio;
		} copdup;
	} v;
	StackOp next;
};

static StackOp iop_new(void){
	new(StackOp, op);
	*op = (struct iop){0};
	return op;
}

#define iop_new_(var,...) StackOp var = iop_new(); if(var) *var = (struct iop)__VA_ARGS__
#define iop_new_bookmark(var) iop_new_(var, {BOOKMARK})
#define iop_new_open(var,stream,cl_rio) iop_new_(var, {IOP_OPEN,{.open={stream,cl_rio}},null})
#define iop_new_close(var,stream,cl_rio) iop_new_(var, {IOP_CLOSE,{.close={stream,cl_rio}},null})
#define iop_new_copy(var,dstream,sstream,cl_rio) iop_new_(var, {IOP_COPY,{.copdup={dstream,sstream,cl_rio}},null})
#define iop_new_dup(var,dstream,sstream,cl_rio) iop_new_(var, {IOP_DUP,{.copdup={dstream,sstream,cl_rio}},null})

#define MAXSTREAMS 256

struct iosstack {
	fd_t state[MAXSTREAMS];
	StackOp top;
};

#define hasstream(stack, s) ((stack)->state[s] > 0)
#define getstream(stack, s) ((stack)->state[s] - 1)
#define setstream(stack, s, io) ((stack)->state[s] = (io)+1)
#define remstream(stack, s) setstream(stack, s, -1)

IOsStack iosstack_new(void){
	new(IOsStack, s);
	*s = (struct iosstack){0};
	return s;
}

void iosstack_destroy(IOsStack s){
	if(!s) return;
	while(s->top) iosstack_pop(s);
	for(size_t i = 0; i < MAXSTREAMS; i++) if(hasstream(s, i)) close(getstream(s, i));
	free(s);
}

IOsStack iosstack_snapdup(IOsStack s){
	if(!s) return null;
	IOsStack ss = iosstack_new();
	if(!ss) return null;
	for(size_t i = 0; i < MAXSTREAMS; i++) if(hasstream(s, i)){
		fd_t d = dup(getstream(s, i));
		if(d < 0) retclean(null, {iosstack_destroy(ss);});
		setstream(ss, i, d);
	}
	return ss;
}

#define stackpush(s,op) { op->next = s->top; s->top = op; }
#define stackpop(s) __extension__({ StackOp _ptop = s->top; s->top = _ptop->next; _ptop; })

Result iosstack_push(IOsStack s){
	if(!s) return Error;
	iop_new_bookmark(b);
	if(!b) return Error;
	stackpush(s, b);
	return Ok;
}

static Result iop_revert(IOsStack s, StackOp op);

Result iosstack_pop(IOsStack s){
	if(!s) return Error;
	while(s->top && s->top->type != BOOKMARK) if(IsOk(iop_revert(s, s->top))) free(stackpop(s)); else return Error;
	if(s->top) free(stackpop(s));
	return Ok;
}

static Result iop_revert(IOsStack s, StackOp op){
	if(!s) return Error;
	if(!op) return Ok;
	switch(op->type){
		case BOOKMARK: break;
		case IOP_OPEN:
			close(getstream(s, op->v.open.stream));
			setstream(s, op->v.open.stream, op->v.open.cl_rio);
			break;
		case IOP_CLOSE:
			setstream(s, op->v.close.stream, op->v.close.cl_rio);
			break;
		case IOP_COPY:
			setstream(s, op->v.copdup.dstream, op->v.copdup.cl_rio);
			break;
		case IOP_DUP:
			close(getstream(s, op->v.copdup.dstream));
			setstream(s, op->v.copdup.dstream, op->v.copdup.cl_rio);
			break;
		default: return Error;
	}
	return Ok;
}

Result iostack_io_open(IOsStack s, fd_t io, fd_t rs){
	if(!s || io < 0 || rs < 0) return Error;
	iop_new_open(op, io, getstream(s, io));
	if(!op) return Error;
	setstream(s, io, rs);
	stackpush(s, op);
	return Ok;
}

Result iostack_io_close(IOsStack s, fd_t io){
	if(!s || io < 0) return Error;
	iop_new_close(op, io, getstream(s, io));
	if(!op) return Error;
	remstream(s, io);
	stackpush(s, op);
	return Ok;
}

Result iosstack_io_copy(IOsStack s, fd_t iodst, fd_t iosrc){
	if(!s || iodst < 0 || iosrc < 0) return Error;
	iop_new_copy(op, iodst, iosrc, getstream(s, iodst));
	if(!op) return Error;
	setstream(s, iodst, getstream(s, iosrc));
	stackpush(s, op);
	return Ok;
}

Result iosstack_io_dup(IOsStack s, fd_t iodst, fd_t iosrc){
	if(!s || iodst < 0 || iosrc < 0) return Error;
	iop_new_dup(op, iodst, iosrc, getstream(s, iodst));
	if(!op) return Error;
	fd_t sio = getstream(s, iosrc);
	fd_t d = sio >= 0 ? dup(sio) : sio;
	if(sio >= 0 && d < 0) retclean(Error, {free(op);});
	setstream(s, iodst, d);
	stackpush(s, op);
	return Ok;
}

