function $(selector) { return document.querySelector(selector); }

window.onload = function () {
    wsInit();

    var strEOJ = "EOJ";
    var dV = 0.1;

    $('#btnStartScan').onclick = function (e) {
        var cmd = {
            cmd: 'scanStart',
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnStopScan').onclick = function (e) {
        var cmd = {
            cmd: 'scanStop',
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

};

function cmdHandler(jCmd) {
    if (jCmd.cmd == "getParam") {
        $('#Fx').value = jCmd.Fx;
        $('#Fy').value = jCmd.Fy;
        $('#Gamma').value = jCmd.Gamma;
        $('#Cx').value = jCmd.Cx;
        $('#Cy').value = jCmd.Cy;
        $('#OfsX').value = jCmd.OfsX;
        $('#OfsY').value = jCmd.OfsY;
        $('#OfsZ').value = jCmd.OfsZ;
    }
}
