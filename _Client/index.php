<!DOCTYPE html>
<meta name="viewport" content="width=device-width; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;" />
<meta name="apple-mobile-web-app-capable" content="yes" />
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent" />

<html>
<head>

<meta charset=utf-8 />

<style type="text/css">
body{
	font-family:Arial, Helvetica, sans-serif;
	overflow: hidden;
	background-color:black;
	}

div {
	display:block;
	position:absolute; 
	color:#444;
	
}
.moveableImage { 
	display:block;
	position:absolute;
	padding:0px;
	margin:0px;
	left:0px;
	top:0px;
}
.largeTypeMessage {
	font-size:22pt; 
	display:block; 
	margin:auto; 
	
	
	
}

</style>
<title>PixelPhones</title>
<script type="text/javascript">
	if ((window==window.top) && 'standalone' in navigator && !navigator.standalone && (/iphone|ipod|ipad/gi).test(navigator.platform) && (/Safari/i).test(navigator.appVersion)) {
		document.write('<link rel="stylesheet" href="css\/add2home.css">');
		document.write('<script type="application\/javascript" src="js\/add2home.js"><\/s' + 'cript>');
	}
	
	if(!("WebSocket" in window)){
		document.write('<script type="text\/javascript" src="swfobject.js"><\/s'+'cript><script type="text\/javascript" src="web_socket.js"><\/s'+'cript>'); 
	}
	
</script>

<script src='js/NyanCatch.js'></script>
<script src='js/MemoryGame.js'></script>


</head>
<body>
	

	
	
	<script type="text/javascript">
 
 	WEB_SOCKET_SWF_LOCATION = "websockets/WebSocketMain.swf";
	WEB_SOCKET_DEBUG = true;

	var host = "ws://"+document.domain+":", 
		portNum = 11999; 
	var socket, canvas, c, msgdiv, msgcount = 0, 
		SCREEN_WIDTH = window.innerWidth, SCREEN_HEIGHT=window.innerHeight, 
		ID = -1, css3d = supports3D(), debugMode = false;

	// binary transmit vars : 
	
	var frameRate = 7,
	blackGaps = true,
	numBits = 4,	
	currentColour,
	broadcastNumber = 0, //Math.floor( Math.random() * Math.pow(2,10)),
	code = [], 
	startTime = (new Date()).getTime(), 
	currentFrame = 0, 
	broadcastIntervalID = -1,
	broadcastStartDelay = 0, 
	blackTimeOffset = 20, // number of extra mils to add to an 'off' signal than an on
	doubleToSingleRatio = 2.5, // how much longer a double is than a single
	currentBroadcastDigitIndex = 0,
	doubleDigit = false,  // true if we're currently broadcasting a double digit
	currentDigit = 0, // the bit that we're currently broadcasting
	lastChangeTime,
	clock, 
	programs, 
	currentProgram,
	longestShortBlack = 0,
	shortestLongBlack = Number.MAX_VALUE,
	longestShortWhite = 0,
	shortestLongWhite = Number.MAX_VALUE, 
	touchesDisabled = false;
	
	
	window.addEventListener("load", init); 
	window.addEventListener("beforeunload", onUnload); 
	
	document.body.addEventListener("touchstart", function(e){ if(touchesDisabled) e.preventDefault()});

	function init(){

		canvas = document.createElement('canvas'); 
		canvas.width = 3; 
		canvas.height = 3; 
		document.body.appendChild(canvas); 
		c = canvas.getContext('2d'); 
		
		setTransform("scale("+(SCREEN_WIDTH)+","+(SCREEN_HEIGHT)+") ");
		//document.body.style.height = SCREEN_HEIGHT+'px';
		
		msgdiv = document.createElement('div'); 
		document.body.appendChild(msgdiv); 
		msgdiv.innerHTML="MSGDIV";
		
		connect();
		
		
		programs = {
			"nyc": new NyanCatch(),
			"mem": new MemoryGame()			
		
		}; 
	
	//	programs.mem.start();
		
	}
	function onUnload() { 
		
		disconnect(); 
	}

	function connect(){
		trace ("trying port #"+ portNum + "...", true);

		portNum++; 
		if(portNum>12000) portNum = 11995;	 

		try{
			socket = new WebSocket(host+portNum);
			message('Socket Status: '+socket.readyState);
			
			socket.onopen = function(){
				message('Socket Status: '+socket.readyState+' (open)');	
				trace('connected');
				socket.send("ready"); 
				clock = new Clock(); 
			
			};

			socket.onmessage = function(msg){
				// may need to split messages with 0xff? 
				
				var msgs = msg.data.split(String.fromCharCode(0xff));
				log(msg.data, msgs); 
				for(var i = 0; i<msgs.length; i++) 
				{

					var msgstr = msgs[i]; 
					
					log("msgstring : " + msgstr);
					if(msgstr.substring(0,1)=='c'){
					
						// CHANGE COLOUR MESSAGE : c<hexcolour>|f<fadespeed>|t<changeTime>
						var changetime = 0; 
						
						// this whole string parsing should def be extracted out into 
						// a method! 
						
						// if it's a synced message : 
						if(msgstr.indexOf('|t')>-1) {
							
							var timestr = msgstr.substring(msgstr.indexOf('|t')+2, msgstr.length);
							
							if(timestr.indexOf('|')>-1) {
								timestr = timestr.substring(0,timestr.indexOf('|')) ; 
							}
							
							log("changetime :", timestr, clock.getServerTime()); 
							
							changetime = parseInt(timestr); 
							changetime -=clock.getServerTime(); 
						
							log("changing in :", changetime); 
								
						}
						
						log('change colour : '+msgstr.substring(1,7)); 
						if(broadcastIntervalID==-1) {
							
							if(msgstr.indexOf('|f')>-1) {
								
								var fadespeed = msgstr.substring(msgstr.indexOf('|f')+2, msgstr.length);
								if(fadespeed.indexOf('|')>-1) { 
									fadespeed = fadespeed.substring(0, fadespeed.indexOf('|')); 
									
								}
								fadespeed = parseFloat(fadespeed); 
								 
								var col = parseInt(msgstr.substring(1,7),16); 
								
								// TODO Add latency for fading! 
								var colFade = new ColourFade(currentColour, col, fadespeed);
							
								
							} else { 
								var colstring = msgstr.substring(1,7);
								if(changetime<=0) {
									changeColour(colstring);
								} else {
									
									var that = this; 
									log("changing in "+changetime); 
									
									setTimeout(function(){changeColour(colstring)}, changetime); 
								
									//setTimeout(function(){that.update.apply(that);}, 10); 
									
								}
							}
						
						}
					} else if(msgstr.substring(0,1)=='i'){
					
					// CHANGE ID MESSAGE : i<idnum>
						
						log('id : '+msgstr.substring(1)); 
						changeID(parseInt(msgstr.substring(1),10)); 
						
					} else if(msgstr.substring(0,1)=='f'){
						
					// FRAMERATE CHANGE : f<framerate>
					
						log('framerate : '+msgstr.substring(1)); 
						frameRate = parseInt(msgstr.substring(1), 10); 
						reset();
						
					} else if(msgstr.substring(0,1)=='n'){

					//  CHANGE NUM OF BITS : n<numbits>

						log('num of bits : '+msgstr.substring(1)); 
						numBits = parseInt(msgstr.substring(1), 10); 
						setNumber(broadcastNumber);
						reset();
						
					} else if(msgstr.substring(0,1)=='r') {
					// CHANGE BLACK TO WHITE RATIO
						log('blackToWhiteRatio = '+msgstr.substring(1));
						blackToWhiteRatio = parseFloat(msgstr.substring(1));
						reset();
					} else if(msgstr.substring(0,1)=='o') {
						
					// CHANGE BLACK TIME OFFSET 
					 	log('blackTimeOffset = '+ msgstr.substring(1));
						blackTimeOffset = parseInt(msgstr.substring(1));
						reset(); 
					} else if(msgstr.substring(0,1)=='t'){

					//  get server time : t<timeinmils>

						log('server time : '+msgstr.substring(1)); 
						clock.receiveServerTime(parseInt(msgstr.substring(1))); 
						
					} else if(msgstr.substring(0,2)=='p:'){
						
					// PROGRAM CHANGE MESSAGE! 
						log('program msg : '+msgstr.substring(2)); 
						
						var progname = msgstr.substring(2); 
						
						if(progname.indexOf("|")>-1) progname = progname.substring(0,progname.indexOf("|")); 
						
						if(programs[progname]) {
							var prog = programs[progname]; 
							
							if(msgstr.indexOf("|start")>-1) {
								
								if(currentProgram) currentProgram.stop(); 
							
								currentProgram = prog; 
								
								currentProgram.start();
							} else if(currentProgram == prog) {
								if (msgstr.indexOf("|stop")>-1){
									currentProgram.stop(); 
									currentProgram = null; 
								} else { 
									
									currentProgram.receiveMessage(msgstr.substring(2+progname.length)); 
								}
								
							} 
							
							
						}
						
					}else if(msgstr=="b1") {
					
					// START BROADCASTING ID : b1
					
						startBroadcastingID(); 
					} else if(msgstr =="b0"){
					
					// STOP BROADCASTING ID : b0
					
						stopBroadcastingID(); 
					} else if(msgstr == "sync") {
						
						clock.start(); 
							
					}
				//	
				}

			};

			socket.onclose = function(){
				message('Socket Status: '+socket.readyState+' (Closed)');
				socket.close(); 
				if(currentProgram) { 
					currentProgram.stop(); 
					currentProgram = null; 
					
				}
				changeColour("000"); 
				stopBroadcastingID(); 
				setTimeout(connect, 1000); 
			};			

		} catch(exception){
			message('Error'+exception);
			setTimeout(connect, 1000); 
		};
		
	
	}

	function message(msg){
		msgcount++; 
	//	if(msgcount%20 == 0) msgdiv.innerHTML="";
		log(msg);
	}


	function disconnect() {
		socket.close();
	}
	
	function changeColour(colString) { 
		while(colString.length<6) colString = "0"+colString; 
		c.fillStyle = "#"+colString; 
		c.fillRect(0,0,4, 4); 
		window.scrollTo(0,1); 
		currentColour = parseInt(colString, 16); 
	}
	function changeID(idNum) { 
	
			ID = idNum; 
			setNumber(ID); 
			msgdiv.innerHTML = "<strong>"+ID+"</strong>";
	}
	
	function sendID() { 
		text = "i"+ID; 
		socket.send(text); 
		
	}
	
	function startBroadcastingID() { 
		log("start broadcasting"); 
		if(broadcastIntervalID!=-1) {
			// we're already broadcasting so clear the interval : 
			stopBroadcastingID(); 
		}
		setNumber(ID); 
		// should probably sort out request animation frame... 
		broadcastIntervalID = 0;
		lastChangeTime = (new Date()).getTime();
		nextChangeTime = 0; 
		currentBroadcastDigitIndex = 0; 
		
		setTransform("scale(0,0)");	
		changeColour("ffffff");
	
		//setTimeout(function() { broadcastIntervalID = setInterval(loop,16);reset(); }, Math.floor(Math.random()*2000)) ; 
		
		//reset();
		broadcastIntervalID = setInterval(loop,16);
		broadcastStartDelay = Math.random()*20; 
		
		log("broadcasting id : "+ID); 
		log(broadcastNumber+"\n"+getBinaryCode(broadcastNumber)+"\n"+code);
		
	}
	
	function stopBroadcastingID() { 
		if(broadcastIntervalID!=-1) {
			clearInterval(broadcastIntervalID); 			
			broadcastIntervalID = -1; 
			setTransform("scale("+SCREEN_WIDTH+","+SCREEN_HEIGHT+")");		
			changeColour("000000");
		}
		
		
	}
	
	////////////////////////////////////////////////////////////////////
	
	//
	
	function loop() {
		if(broadcastStartDelay-->0) return; 
		var now = Date.now();// || (new Date()).getTime());
		var elapsedTime =   now - lastChangeTime; 
		
		if(elapsedTime>=nextChangeTime) {
			
			
			// check elapsed time to make sure it's not screwed up
			
			if((currentDigit == "0") && doubleDigit) {
				if(elapsedTime < shortestLongBlack) {
					shortestLongBlack = elapsedTime; 
					log("shortestLongBlack : ", shortestLongBlack);
				}
				if(elapsedTime > longestShortBlack) {
					reset(); 
				}
			} else if((currentDigit =="0") &&!doubleDigit) {
				
				if(elapsedTime > longestShortBlack) {
					longestShortBlack = elapsedTime; 
					log("longestShortBlack : ", longestShortBlack);
				}
				if(elapsedTime < shortestLongBlack) {
					reset(); 
				}	
			} else if((currentDigit == "1") && doubleDigit) {
				if(elapsedTime < shortestLongWhite) {
					shortestLongWhite = elapsedTime; 
					log("shortestLongWhite : ", shortestLongWhite);
				}
				if(elapsedTime > longestShortWhite) {
					reset(); 
				}
			}else if((currentDigit =="1") &&!doubleDigit) {

				if(elapsedTime > longestShortWhite) {
					longestShortWhite = elapsedTime; 
				}
				if(elapsedTime < shortestLongWhite) {
					reset(); 
				}
			}
				
			// get next digit and set the next changetime
			currentBroadcastDigitIndex ++; 
			
			// if we're at the end, tell the server and stop. 
			if(currentBroadcastDigitIndex>code.length) {
				// if you'd rather just restart, then use : currentBroadcastDigitIndex = 0; 
				// b0 tells the server we're finished. 
				console.log('stopping broadcasting');
				socket.send('b0');
				stopBroadcastingID(); 
				return; 
			
			}
			
			// get the next bit to broadcast, 0 or 1
			var digit = currentDigit = code[currentBroadcastDigitIndex]; 
			
			// and if there are two in a row, let's make it a double!
			if((currentBroadcastDigitIndex<code.length-1) && ( digit == code[currentBroadcastDigitIndex+1])) {
				currentBroadcastDigitIndex++; 
				doubleDigit = true; 
			} else {		
				doubleDigit = false; 
			}
			
			var framelength = 1000 / frameRate;
			//lastChangeTime += elapsedTime -(elapsedTime - nextChangeTime); 
			lastChangeTime = now; 
			nextChangeTime = framelength; 
			if(doubleDigit) nextChangeTime*=doubleToSingleRatio; 
			if(digit=="0") nextChangeTime += blackTimeOffset; 
			
			
			if(digit == "1") { 
				setTransform( "scale("+SCREEN_WIDTH+","+SCREEN_HEIGHT+")");		

			} else {

				setTransform("scale(0,0)");		
				//context.clearRect(0,0,2, 2);
			}
			
			//lastChangeTime +=elapsedTime; 
			
		}
		
		
		/*
		var currentTime = (new Date()).getTime() - startTime; 
		var frame = Math.floor(currentTime / (1000/frameRate)); 
		//	log(currentTime, frame); 
		if(frame>currentFrame) {
		
			
			
			if(code[frame%code.length]==1) { 
				setTransform( "scale("+SCREEN_WIDTH+","+SCREEN_HEIGHT+")");		
				
			} else {
			
				setTransform("scale(0,0)");		
				//context.clearRect(0,0,2, 2);
			}
				
			currentFrame = frame; 
			//log(currentDigit, flashWhite); 
			
		}
		*/
	}
	
	function reset() { 
		startTime = (new Date()).getTime(); 
		currentFrame =0; 
		longestShortBlack = 0; 
		shortestLongBlack = Number.MAX_VALUE;
		longestShortWhite = 0; 
		shortestLongWhite = Number.MAX_VALUE;
	}
	
	
	function setNumber(num) {
		broadcastNumber = Number(num); 
		code = getManchesterCode(getBinaryCode(num, numBits));
		//infoDiv.innerHTML = broadcastNumber+"<br>"+getBinaryCode(num)+"<br>"+code;
		
		
	}
	
	
	function getBinaryCode(num, padding) {
		
		//code = [true]; 
		binaryString = num.toString(2); 
		
		
		while(binaryString.length<padding) 
			binaryString = "0"+binaryString; 
		
		while(binaryString.length>padding) 
			binaryString = binaryString.substring(1, binaryString.length); 
		
		// alternate code for producing checksum instead of duplicating ID
		var checksum = ""; 
		
		for(var i = 0; i<binaryString.length; i+=2) { 
			if(i+1<binaryString.length) { 
				if(binaryString[i] == binaryString[i+1]) { 
					checksum +="0"; 
				} else { 
					checksum +="1"; 
				}	
			} else { 
				checksum +=binaryString[i]; 
			}
		}
		log(binaryString + " "+ checksum); 
			
		if(blackGaps)
			return "1"+binaryString+checksum+"0"; 
		else
			return "0"+binaryString+checksum+"1"; 
		
	
		
	}
	
	function getManchesterCode(binaryString) {
		var manchesterCode = []; 
		
	
		for(var j = 0; j<binaryString.length; j++) { 
			if(binaryString.charAt(j)=="0")
				manchesterCode.push(0, 1);
			else
				manchesterCode.push(1, 0);
			 
		}
		if(blackGaps)
			manchesterCode.unshift(0,0,0,0,0,0,0,0,0,0); 
		else
			manchesterCode.unshift(1,1,1,1,1,1,1,1,1,1); 
			
		return manchesterCode; 

		
	}
	
	function trace(msg, clear) {
		if(clear) msgdiv.innerHTML = "";  
		msgdiv.innerHTML=msg+"<br />" + msgdiv.innerHTML; 	
		log(msg); 
	}
	
	
	function ColourFade(from, to, durationSecs) {
		
		this.from = from; 
		this.to = to; 
		this.durationMils = durationSecs*1000; 
		this.startTime = new Date().getTime(); 
		
	//	trace("ColourFade : "+from.toString(16)+" "+(typeof from)+" "+to+" "+(typeof to)+" "+durationSecs+" "+ (typeof durationSecs)); 
			
		this.update = function() { 
			var progress = (new Date().getTime() - this.startTime) / this.durationMils; 
			//log(progress, this.startTime, this.durationMils); 
			if(progress>=1) { 
				changeColour(this.to.toString(16)); 	
			} else {
				changeColour(getInterpolatedColour(this.from, this.to, progress).toString(16)); 
			
				var that = this; 
			
				setTimeout(function(){that.update.apply(that);}, 10); 
				
			}
		
		};
		this.update(); 
	}
	
	function getInterpolatedColour(from, to, progress) { 
		
		var rfrom = from>>16; 
		var rto = to>>16; 
		
		var gfrom = (from >> 8 ) & 0xff; 
		var gto = (to >> 8 ) & 0xff; 

		var bfrom = from  & 0xff; 
		var bto = to & 0xff; 
		
		var r = ((rto-rfrom) * progress) + rfrom; 
		var g = ((gto-gfrom) * progress) + gfrom; 
		var b = ((bto-bfrom) * progress) + bfrom;
		
		return (r<<16) | (g<<8) | (b);  
		
	}
	
	function setTransform(transString) {
		var s = canvas.style
		s.webkitTransform = s.MozTransform = s.OTransform = s.transform = transString + (css3d ? " translateZ(0px)" : ""); 	
		
		
	}
	function supports3D() {
		// borrowed from modernizr
		var div = document.createElement('div'),
			ret = false,
			properties = ['perspectiveProperty', 'WebkitPerspective'];
		for (var i = properties.length - 1; i >= 0; i--){
			ret = ret ? ret : div.style[properties[i]] != undefined;
		};
	 
	    // webkit has 3d transforms disabled for chrome, though
        //   it works fine in safari on leopard and snow leopard
        // as a result, it 'recognizes' the syntax and throws a false positive
        // thus we must do a more thorough check:
        if (ret){
            var st = document.createElement('style');
            // webkit allows this media query to succeed only if the feature is enabled.    
            // "@media (transform-3d),(-o-transform-3d),(-moz-transform-3d),(-ms-transform-3d),(-webkit-transform-3d),(modernizr){#modernizr{height:3px}}"
            st.textContent = '@media (-webkit-transform-3d){#test3d{height:3px}}';
            document.getElementsByTagName('head')[0].appendChild(st);
            div.id = 'test3d';
            document.body.appendChild(div);

            ret = div.offsetHeight === 3;

            st.parentNode.removeChild(st);
            div.parentNode.removeChild(div);
        }
        return ret;
	}
	
	
	
	
	
	function Clock() { 
		
		this.pendingRequest = false; 
		this.timeRequestSent; 
		this.deltas = []; 
		this.latency = 0; 
		this.lockedIn; 
		this.syncTimeDelta; 
		this.maxDeltas = 10; 
		
		// start pinging! 
		this.start = function() {
			
			
			this.requestServerTime(); 
			
			changeColour("ff0000"); 
			
			
		}
		
		this.requestServerTime = function() { 
			
			var now = new Date().getTime(); 
			//send the relevant message to the server.
			socket.send("t"+now); 
			this.pendingRequest = true;
			this.timeRequestSent = now;  
			
		}
		
		this.receiveServerTime = function(serverTime) { 
			
			var now = new Date().getTime(); 
			this.addTimeDelta(this.timeRequestSent, now, serverTime);
			this.pendingRequest = false; 
			
			if(this.deltas.length < this.maxDeltas) this.requestServerTime(); 
		}
		
		this.addTimeDelta = function(clientSendTime, clientRecTime, serverRecTime) { 
			
			
			var latency = (clientRecTime-clientSendTime)/2; 
			var clientServerDelta = serverRecTime-clientRecTime;
			
			var timeSyncDelta = clientServerDelta +latency; 
			var delta = new TimeDelta(latency, timeSyncDelta); 
			
			//trace(clientSendTime +" "+ serverRecTime);//latency); 
			this.deltas.push(delta); 
			
			this.recalculate(); 
			
			
			log("test server time: ", this.getServerTime() - serverRecTime);
			
		}
		
		this.recalculate = function() { 
		
			var tmpdeltas = this.deltas.slice(0); 
			tmpdeltas.sort(function(a,b){return a.latency>b.latency}); 
			log("sorted", tmpdeltas.length)
			log(tmpdeltas); 
			
			var medianindex = Math.floor(tmpdeltas.length/2); 
			var medianLatency = tmpdeltas[medianindex].latency; 
			
			var maxValue = medianLatency * 1.5; 
			var i =0; 
			while((i<tmpdeltas.length) && (tmpdeltas[i].latency<=maxValue)) i++ ; 
			tmpdeltas.splice(i); 
			
			
			log("spliced", tmpdeltas.length, medianLatency, maxValue);
			log(tmpdeltas, tmpdeltas.length); 
			this.latency =0; 
			for(var i =0; i<tmpdeltas.length; i++) { 
				this.latency+=tmpdeltas[i].latency; 
			} 
			
			this.latency/=tmpdeltas.length; 
			
			log("average latency = " + this.latency); 
			
			if(!this.lockedIn) {
				
				var averageDeltas = 0; 
				for(var i=0; i< this.deltas.length;i++) { 
					averageDeltas +=this.deltas[i].timeSyncDelta; 
				}
				averageDeltas /= this.deltas.length; 
				
				this.syncTimeDelta = Math.round(averageDeltas); 
				
				this.lockedIn = (this.deltas.length == this.maxDeltas); 
				
				if(this.lockedIn) {
					socket.send("sync"+Math.round(this.latency)); 
					changeColour("ffff00"); 
				}
			}
			
			
			
		}
		this.getServerTime = function() {
			var now = new Date().getTime(); 
			return now+this.syncTimeDelta; 
			
		}
		
		
	}
	

	
	function TimeDelta(latency, timeSyncDelta) { 
		this.latency = latency; 
		this.timeSyncDelta = timeSyncDelta; 
		
	}
	
	
	
	
	function log() { 
		
		if(debugMode) console.log(Array.prototype.slice.call(arguments));
		
		
	}
	

	
	
	
	</script>  

        
  </body>
</html>​