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
            speed: $('#iptXspeed').value,
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
            speed: $('#iptXspeed').value,
            pTarget: -1,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnXincStep').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: 1,
            speed: parseFloat($('#iptXspeed').value),
            pTarget: parseFloat($('#Xpos').value) + parseFloat($('#iptXstep').value),
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnXdecStep').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: -1,
            speed: parseFloat($('#iptXspeed').value),
            pTarget: parseFloat($('#Xpos').value) - parseFloat($('#iptXstep').value),
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };


    $('#btnYinc').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: 1,
            speed: $('#iptYspeed').value,
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
            speed: $('#iptYspeed').value,
            pTarget: -1,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnYincStep').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: 1,
            speed: parseFloat($('#iptYspeed').value),
            pTarget: parseFloat($('#Ypos').value) + parseFloat($('#iptYstep').value),
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnYdecStep').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: -1,
            speed: parseFloat($('#iptYspeed').value),
            pTarget: parseFloat($('#Ypos').value) - parseFloat($('#iptYstep').value),
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
