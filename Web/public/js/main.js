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
    freqTable = new FrequencyTable({control:remote});
    
    //Bind status
    status = new StatusView({ el: '#status', model:remote});
    
    //Bind debug window
    debugWindow = new DebugView({ el: '#dbgwin', model:remote});

    //Bind decoder window
    demodWindow = new DecoderView({ el: '#dmodwin', model:remote});
    
    //Bind radio control buttons
    radioCmd = new RadioCmd({ el: '#control', model:remote});

    //Bind radio control buttons
    radioCmd = new ControlsCmd({ el: '#controlleft', model:remote});
	
    //Bind radio control buttons
    radioDisplay = new Display({ el: '#mainDisplay', frequencytable:freqTable, model:remote});
    
    // Viewer
    viewer = new ViewerView2D({ el : '#scope', model:remote});
    
    // Connect remote to device
    remote.connect();
	
    // Connect sound streaming
    sound.connect();

});
