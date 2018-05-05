
function Session (url, handler) {
    var socket = new WebSocket(url);
    socket.binaryType = 'arraybuffer';      // binary data, msgpack format
    var self = this;
    socket.onopen = function (event) {
        if (handler._onopen)
            handler._onopen(self);
    };
    socket.onmessage = function (event) {
        var arr = new Uint8Array(event.data);
        var data = msgpack.decode(arr);
        self.handle_pack(data);
    };
    socket.onerror = function (event) {
        if (handler._onerror)
            handler._onerror(self);
    };
    socket.onclose = function (event) {
        if (handler._onclose)
            handler._onclose(self);
    };
    // socket.session = this;
    this.socket = socket;           // Session关联的WebSocket
    this.handler = handler;         // Session的处理器
    this.id = 1;                    // id counter
    this.callback = {};             // callback's table
    return this;
}
// store the callback, and return the message's id
Session.prototype.store_callback = function (callback) {
    if (callback) {
        this.callback[this.id] = callback;
        return this.id++;
    }
    return null;
}
// get the callback by message's id
Session.prototype.get_callback = function (id) {
    var callback = this.callback[id];
    if (callback) {
        delete this.callback[id];
        return callback;
    }
    return null;
}
// handle the data pack
Session.prototype.handle_pack = function (data) {
    if (data[0]) {                  // pack 'call'
        var id = data[1];
        var name = data[2];
        this.handle_call(id, name, data.slice(3));
    } else {                        // pack 'return'
        var id = data[1];
        this.handle_return(id, data[2]);
    }
}
// handle the pack 'call'
Session.prototype.handle_call = function (id, name, args) {
    var fn = this.handler[name];
    var ret = null;
    if (fn)
        ret = fn(self, args);
    if (id)     // is a pack 'call' but 'signal'
        this.retn(id, args);
}
// handle the pack 'return'
Session.prototype.handle_return = function (id, pack) {
    var callback = this.get_callback(id);
    if (callback)
        callback(this, pack);
}
// send a pack
Session.prototype.send = function (pack) {
    var ui8 = msgpack.encode(pack)
    this.socket.send
        (ui8.buffer.slice(0, ui8.byteLength));
};
// call
Session.prototype.call = function (name) {
    var pack = [true, 0, name];
    var callback = null;
    for (var i = 1; i < arguments.length; i++)
        if (typeof arguments[i] == 'function') {
            callback = arguments[i];
            break;
        } else {
            pack.push(arguments[i]);
        }
    if (callback)
        pack[1] = this.store_callback(callback);
    this.send(pack);
}
// return
Session.prototype.retn = function (id, args) {
    this.send([false, id, args]);
}
