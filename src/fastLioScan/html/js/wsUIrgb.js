function $(selector) { return document.querySelector(selector); }

window.onload = function () {
    wsInit();

    var strEOJ = "EOJ";
    var dV = 0.1;

    $('#btnLoadProj').onclick = function (e) {
        var cmd = {
            cmd: 'loadProj',
            projDir: $('#projDir').value,
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

    // $('#btnGetCamConfigSaved').onclick = function (e) {
    //     var cmd = {
    //         cmd: 'getCamConfigSaved',
    //     };

    //     cmdStr = JSON.stringify(cmd) + strEOJ;
    //     wsSocket.send(cmdStr);
    // };

    $('#btnSaveCamConfig').onclick = function (e) {
        var cmd = {
            cmd: 'saveCamConfig',
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

    $('#vCStXInc').onclick = function (e) {
        $('#vCStX').value = parseFloat($('#vCStX').value) + dV;
        cmdSetCamConfig();
    };

    $('#vCStXDec').onclick = function (e) {
        $('#vCStX').value = parseFloat($('#vCStX').value) - dV;
        cmdSetCamConfig();
    };

    $('#vCStYInc').onclick = function (e) {
        $('#vCStY').value = parseFloat($('#vCStY').value) + dV;
        cmdSetCamConfig();
    };

    $('#vCStYDec').onclick = function (e) {
        $('#vCStY').value = parseFloat($('#vCStY').value) - dV;
        cmdSetCamConfig();
    };

    $('#vCStZInc').onclick = function (e) {
        $('#vCStZ').value = parseFloat($('#vCStZ').value) + dV;
        cmdSetCamConfig();
    };

    $('#vCStZDec').onclick = function (e) {
        $('#vCStZ').value = parseFloat($('#vCStZ').value) - dV;
        cmdSetCamConfig();
    };

    
    $('#vCSrXInc').onclick = function (e) {
        $('#vCSrX').value = parseFloat($('#vCSrX').value) + dV;
        cmdSetCamConfig();
    };

    $('#vCSrXDec').onclick = function (e) {
        $('#vCSrX').value = parseFloat($('#vCSrX').value) - dV;
        cmdSetCamConfig();
    };

    $('#vCSrYInc').onclick = function (e) {
        $('#vCSrY').value = parseFloat($('#vCSrY').value) + dV;
        cmdSetCamConfig();
    };

    $('#vCSrYDec').onclick = function (e) {
        $('#vCSrY').value = parseFloat($('#vCSrY').value) - dV;
        cmdSetCamConfig();
    };

    $('#vCSrZInc').onclick = function (e) {
        $('#vCSrZ').value = parseFloat($('#vCSrZ').value) + dV;
        cmdSetCamConfig();
    };

    $('#vCSrZDec').onclick = function (e) {
        $('#vCSrZ').value = parseFloat($('#vCSrZ').value) - dV;
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

    $('#vCStX').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#vCStY').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#vCStZ').oninput = function (e) {
        cmdSetCamConfig();
    };


    $('#vCSrX').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#vCSrY').oninput = function (e) {
        cmdSetCamConfig();
    };

    $('#vCSrZ').oninput = function (e) {
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
            vCStX: parseFloat($('#vCStX').value),
            vCStY: parseFloat($('#vCStY').value),
            vCStZ: parseFloat($('#vCStZ').value),
            vCSrX: parseFloat($('#vCSrX').value),
            vCSrY: parseFloat($('#vCSrY').value),
            vCSrZ: parseFloat($('#vCSrZ').value),
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
        $('#vCStX').value = jCmd.vCStX;
        $('#vCStY').value = jCmd.vCStY;
        $('#vCStZ').value = jCmd.vCStZ;
        $('#vCSrX').value = jCmd.vCSrX;
        $('#vCSrY').value = jCmd.vCSrY;
        $('#vCSrZ').value = jCmd.vCSrZ;
    }
}
