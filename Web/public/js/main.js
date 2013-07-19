$(function () {
    $('#Tab a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    })    
})

$(document).ready(function(){

    lastcmd = $('#lastcmd');

    // Build remote control 
    remote = new RemoteControl();
	
    // Build sound streaming 
    sound = new SoundControl();
    
    //Build Frequency Table maangement
    freqTable = new FrequencyTable();
    
    //Bind remote control signal strength
    signal = new SignalStrength({ el: '#signal', model:remote});
    
    //Bind status
    status = new StatusView({ el: '#status', model:remote});
    
    //Bind debug window
    debugWindow = new DebugView({ el: '#dbgwin', model:remote});

    //Bind decoder window
    demodWindow = new DecoderView({ el: '#dmodwin', model:remote});
    
    //Bind radio control buttons
    radioCmd = new RadioCmd({ el: '#control', model:remote});
    
    // Bind frequency
    frequency = new FrequencyView({ el: '#frequency', frequencytable:freqTable, model:remote });
    frequencystep = new FrequencyStepView({ el: '#frequencystep', frequencytable:freqTable, model:remote });

    // Viewer
    viewer = new ViewerView2D({ el : '#scope', model:remote});
    
    // Connect remote to device
    remote.connect();
	
    // Connect sound streaming
    sound.connect();

});
