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

    $('#btnExportModel').onclick = function (e) {
        var cmd = {
            cmd: 'exportModel',
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnGetCamConfig').onclick = function (e) {
        var cmd = {
            cmd: 'getCamConfig',
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnGetCamConfigSaved').onclick = function (e) {
        var cmd = {
            cmd: 'getCamConfigSaved',
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#FxInc').onclick = function (e) {
        $('#Fx').value = parseFloat($('#Fx').value) + dV;
        cmdSetCamConfig();
    };

    $('#FxDec').onclick = function (e) {
        $('#Fx').value = parseFloat($('#Fx').value) - dV;
        cmdSetCamConfig();
    };

    $('#FyInc').onclick = function (e) {
        $('#Fy').value = parseFloat($('#Fy').value) + dV;
        cmdSetCamConfig();
    };

    $('#FyDec').onclick = function (e) {
        $('#Fy').value = parseFloat($('#Fy').value) - dV;
        cmdSetCamConfig();
    };

    $('#GammaInc').onclick = function (e) {
        $('#Gamma').value = parseFloat($('#Gamma').value) + dV;
        cmdSetCamConfig();
    };

    $('#GammaDec').onclick = function (e) {
        $('#Gamma').value = parseFloat($('#Gamma').value) - dV;
        cmdSetCamConfig();
    };

    $('#CxInc').onclick = function (e) {
        $('#Cx').value = parseFloat($('#Cx').value) + dV;
        cmdSetCamConfig();
    };

    $('#CxDec').onclick = function (e) {
        $('#Cx').value = parseFloat($('#Cx').value) - dV;
        cmdSetCamConfig();
    };

    $('#CyInc').onclick = function (e) {
        $('#Cy').value = parseFloat($('#Cy').value) + dV;
        cmdSetCamConfig();
    };

    $('#CyDec').onclick = function (e) {
        $('#Cy').value = parseFloat($('#Cy').value) - dV;
        cmdSetCamConfig();
    };

    $('#OfsXInc').onclick = function (e) {
        $('#OfsX').value = parseFloat($('#OfsX').value) + dV;
        cmdSetCamConfig();
    };

    $('#OfsXDec').onclick = function (e) {
        $('#OfsX').value = parseFloat($('#OfsX').value) - dV;
        cmdSetCamConfig();
    };

    $('#OfsYInc').onclick = function (e) {
        $('#OfsY').value = parseFloat($('#OfsY').value) + dV;
        cmdSetCamConfig();
    };

    $('#OfsYDec').onclick = function (e) {
        $('#OfsY').value = parseFloat($('#OfsY').value) - dV;
        cmdSetCamConfig();
    };

    $('#OfsZInc').onclick = function (e) {
        $('#OfsZ').value = parseFloat($('#OfsZ').value) + dV;
        cmdSetCamConfig();
    };

    $('#OfsZDec').onclick = function (e) {
        $('#OfsZ').value = parseFloat($('#OfsZ').value) - dV;
        cmdSetCamConfig();
    };


    $('#Fx').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#Fy').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#Gamma').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#Cx').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#Cy').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#OfsX').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#OfsY').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#OfsZ').oninput = function (e) {
        cmdSetCamConfig();
    };


    function cmdSetCamConfig()
    {
        var cmd = {
            cmd: 'setCamConfig',
            Fx: parseFloat($('#Fx').value),
            Fy: parseFloat($('#Fy').value),
            Gamma: parseFloat($('#Gamma').value),
            Cx: parseFloat($('#Cx').value),
            Cy: parseFloat($('#Cy').value),
            OfsX: parseFloat($('#OfsX').value),
            OfsY: parseFloat($('#OfsY').value),
            OfsZ: parseFloat($('#OfsZ').value),
        };

        cmdStr = JSON.stringify(cmd) + strEOJ;
        wsSocket.send(cmdStr);
    };

    $('#btnSaveCamConfig').onclick = function (e) {
        var cmd = {
            cmd: 'saveCamConfig',
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
