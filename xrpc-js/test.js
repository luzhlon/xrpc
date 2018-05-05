
var ss = new Session('ws://127.0.0.1:5333', {
    _onopen : function(session) {
        // session.call('CreateProcess', 'C:\\Windows\\notepad.exe')
        session.call('CreateProcess', 'C:\\Windows\\notepad.exe')
    },
    _onclose : function(session) {
        console.log(session, 'closed');
    },
    onoutput : function(rpc, args) {
        console.log(args[0]);
    },
    echo : function(session, args) {
        return args;
    }
});
