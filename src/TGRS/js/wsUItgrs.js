function $(selector) { return document.querySelector(selector); }

window.onload = function () {
    wsInit();

    var strEOJ = "EOJ";
    var dV = 0.1;

    // $('#btnStop').onclick = function (e) {
    //     var cmd = {
    //         cmd: 'stop',
    //     };

    //     cmdStr = JSON.stringify(cmd) + strEOJ;
    //     wsSocket.send(cmdStr);
    // };

    $('#btnXinc').onclick = function (e) {
        var Xpos = parseInt($('#iptXpos').value) + parseInt($('#iptXstep').value);
        $('#iptXpos').value = Xpos;

        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: 1,
            speed: parseInt($('#iptXspeed').value),
            pTarget: Xpos,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnXdec').onclick = function (e) {
        var Xpos = parseInt($('#iptXpos').value) - parseInt($('#iptXstep').value);
        $('#iptXpos').value = Xpos;

        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: -1,
            speed: parseInt($('#iptXspeed').value),
            pTarget: Xpos,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnXgoto').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'x',
            direction: 1,
            speed: parseInt($('#iptXspeed').value),
            pTarget: parseInt($('#iptXgoto').value),
        };

        $('#iptXpos').value = $('#iptXgoto').value;

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };


    $('#btnYinc').onclick = function (e) {
        var Ypos = parseInt($('#iptYpos').value) + parseInt($('#iptYstep').value);
        $('#iptYpos').value = Ypos;

        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: 1,
            speed: parseInt($('#iptYspeed').value),
            pTarget: Ypos,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnYdec').onclick = function (e) {
        var Ypos = parseInt($('#iptYpos').value) - parseInt($('#iptYstep').value);
        $('#iptYpos').value = Ypos;

        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: -1,
            speed: parseInt($('#iptYspeed').value),
            pTarget: Ypos,
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnYgoto').onclick = function (e) {
        var cmd = {
            cmd: 'move',
            axis: 'y',
            direction: 1,
            speed: parseInt($('#iptYspeed').value),
            pTarget: parseInt($('#iptYgoto').value),
        };

        $('#iptYpos').value = $('#iptYgoto').value;

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
