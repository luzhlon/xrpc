
#include <thread>
#include <assert.h>

#include "xval_mp.h"
#include "xrpc.h"

namespace xrpc {
    using namespace xval;

    void RPCHandler::onopen(RPCHandler& h) {
        if (h.onOpen) h.onOpen(h);
    }

    void RPCHandler::onclose(RPCHandler& h) {
        if (h.onClose) h.onClose(h);
    }
    void RPCHandler::onerror(RPCHandler& h) {
    }

    void RPCHandler::onmessage(RPCHandler& h) {
        h.parse_pack();
        assert(h.iscall());     // must be a call message
        h.handle_call();        // message id, function id
    }

    void RPCHandler::parse_pack() {
        assert(mp_unpack(_pack, data()));
        assert(_pack.islist());
    }

    void RPCHandler::handle_call() {
        auto& pack = _pack.list();
        auto msgid = pack.get(1);       // message id
        auto funid = pack.get(2);       // function id
        // call the function
        auto func = getFunc(funid);
        Value ret; Value& args = ret;
        if (func) // get the args
            args = Tuple::New(pack.begin()+3, pack.size() - 3),
            ret = func(*this, args.tuple());
        // return message, this is a signal if msgid == 0
        if (msgid) retn(msgid, ret);
    }

    Value RPCHandler::invoke(const Value& func, const Value *argv, size_t argc, bool signal) {
        // construct the pack
        auto& pack = _pack.list();
        pack.resize(3);
        pack.set(0, true);
        pack.set(1, signal ? 0 : _idnum++);
        pack.set(2, func);
        for (size_t i = 0; i < argc; i++)
            pack.append(argv[i]);
        send_pack();
        // wait return
        return signal ? Value::Nil : wait_return();
    }

    Value RPCHandler::wait_return() {
        do {
            recv();
            if (!isConnected())
                break;
            parse_pack();
            if (isreturn())
                return _pack.get(2);
            else
                handle_call();
        } while (true);
        return Value::Nil;
    }

    bool RPCHandler::send_pack() {
        data().resize(0);
        mp_pack(_pack, data());
        return send(data(), true);
    }

    bool RPCHandler::retn(const Value& msgid, const Value& v) {
        auto& pack = _pack.list();
        pack.resize(3);
        pack.set(0, false);
        pack.set(1, msgid);
        pack.set(2, v);
        return send_pack();
    }

    RPCServer::RPCServer(const char *addr, unsigned short port)
        : _RPCServerBase(addr, port) {}
    RPCServer::RPCServer(unsigned short port)
        : _RPCServerBase(port) {}

    RPCHandler& RPCServer::accept() { return _RPCServerBase::accept(); }
    bool RPCServer::run() { return _RPCServerBase::run(); }
}
