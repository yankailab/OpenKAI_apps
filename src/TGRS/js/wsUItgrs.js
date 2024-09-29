function $(selector) { return document.querySelector(selector); }

window.onload = function () {
    wsInit();

    var strEOJ = "EOJ";
    var dV = 0.1;

    $('#btnStop').onclick = function (e) {
        var cmd = {
            cmd: 'stop',
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };


    $('#btnXinc').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: 1,
            speed: parseFloat($('#iptXspeed').value),
            pTarget: -1,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnXdec').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: -1,
            speed: parseFloat($('#iptXspeed').value),
            pTarget: -1,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnXgoto').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: 1,
            speed: parseFloat($('#iptXspeed').value),
            pTarget: parseFloat($('#iptXgoto').value),
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };
    

    $('#btnYinc').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: 1,
            speed: parseFloat($('#iptYspeed').value),
            pTarget: -1,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnYdec').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: -1,
            speed: parseFloat($('#iptYspeed').value),
            pTarget: -1,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnYgoto').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: 1,
            speed: parseFloat($('#iptYspeed').value),
            pTarget: parseFloat($('#iptYgoto').value),
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

};

function cmdHandler(jCmd) {
    if (jCmd.cmd == "updatePos") {
        $('#Xpos').value = jCmd.pX;
        $('#Ypos').value = jCmd.pY;
    }
}
