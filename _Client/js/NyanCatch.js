function NyanCatch() { 
	
	var loopInterval, 
		nyanImage,
		showTime,
		speed,
		caught,
		counter,
		msgDiv = document.createElement('div');
	
	msgDiv.className = "largeTypeMessage"; 
	
	
	this.start = function() { 
		log("start nyan catch!"); 
		if(!nyanImage) {
			nyanImage = new Image();
			nyanImage.onload = imageLoaded; 
			nyanImage.src = "nyc/nyanCat.gif";
			nyanImage.className = "moveableImage";	
		}
		
		
		this.reset(); 
		
		
		loopInterval = setInterval(update, 1000/60); 
		changeColour("000066"); 
		
		
	};
	
	
	this.reset = function() {
	
		showTime = -1; 
		speed = -1; 
		caught = false;
		counter = 0; 
		msgDiv.width = SCREEN_WIDTH; 
		msgDiv.style.top = "100px"; 
		changeColour("0000ff"); 
		removeDiv(); 
		
		
		
	}
	
	
	this.stop = function() { 
		
		clearInterval(loopInterval); 
		removeCat(); 
		removeDiv(); 
		changeColour("000000"); 
	};
	
	
	function update () {
		
		counter++; 
		//log("showTime", clock.getServerTime()-showTime); 
		if(showTime<0) return;
		var time = clock.getServerTime(); 
		
		var progress = (time - showTime) / speed; 
			
		if((progress>=0) && (progress<=1)) {
			showCat(); 
			var distance = SCREEN_WIDTH + (nyanImage.width*2); 
			var progress = (time - showTime) / speed; 
			nyanImage.style.left = ((progress * distance)-nyanImage.width)+"px"; 
			nyanImage.style.top = ((SCREEN_HEIGHT/2) - (nyanImage.height/2)) +"px";
			//log("showing kitty at ", progress, (progress * distance)); 
			touchesDisabled = true; 
		} else {
			removeCat(); 
			touchesDisabled = false; 
		}
		
		if((caught) && (counter%10==0)) { 
			
			var col = Math.floor(Math.random()*6) +1;
			var hex = ((col & 4)>0) ? 0xff0000 : 0x000000; 
			hex = hex | (((col & 2)>0) ? 0x00ff00 : 0x000000);
			hex = hex | (((col & 1)>0) ? 0x0000ff : 0x000000);
			//log(hex.toString(16), col, col & 4, col & 2, col & 1); 
			changeColour(hex.toString(16)); 
			
			
		}
		
	}
	
	function showCat () { 
		//log("showCat", nyanImage)
		if(nyanImage.parentNode!=document.body) {
			document.body.appendChild(nyanImage); 
			if(css3d) 
				nyanImage.style.webkitTransform = "translateZ(0px)"; 
			
		}
		
		
	}
	
	function removeCat() { 			
		if(nyanImage.parentNode == document.body)
			document.body.removeChild(nyanImage);

	};
	function removeDiv () { 			
		if(msgDiv.parentNode == document.body)
			document.body.removeChild(msgDiv);

	};
	function showMessage(msg) { 
		msgDiv.innerHTML = msg; 
		
		if(msgDiv.parentNode != document.body)
			document.body.appendChild(msgDiv);
		
	}
	
	this.receiveMessage = function(msg) { 
		
		log(msg); 
		
		if(msg.indexOf("reset")>-1) { 
			
			this.reset(); 
			
		}
		else if(msg.indexOf("show")>-1){
			var msg = msg.substr(msg.indexOf("show|")+5); 
			var params = msg.split("|"); 
			//log(msg); 
			log("showtime ", params[0],params[1]);
			showTime = params[0]; 
			speed = params[1]; 
		
		}
		else if(msg.indexOf("caught")>-1){
			var caughtTime = msg.substr(msg.indexOf("caught|")+7); 
		 	changeColour("ff0000"); 
			caught = true; 
		}
		else if(msg.indexOf("place")>-1) { 
			
			var place = msg.substr(msg.indexOf("place|")+6); 
			showMessage("You rank #"+(parseInt(place)+1)+"!!!!<br>YAY!"); 
			
		} else if(msg.indexOf("missed")>-1) { 
			
			showMessage("You missed the kitty ;-("); 
			
		}
		
	}
	
	
	function imageLoaded() { 
		log('imageLoaded');
		//document.body.appendChild(this.img);
		//var that = this; 
		nyanImage.addEventListener("touchstart", touchStart);
		nyanImage.addEventListener("mousedown", mouseDown);
		//this.img.addEventListener("touchmove", function(e){ that.touchMove.call(that,e)});
		//this.img.addEventListener("touchend", function(e){ that.touchEnd.call(that,e)});
		
		//this.audioCheckInterval = setInterval(function(e){ that.checkAudio.call(that)}, 1);
	//this.showCat(); 
	}
	
	function touchStart(e) { 
		e.preventDefault(); 
		
		for (var i=0;i<e.changedTouches.length;i++) { 
			var touch = e.changedTouches[i]; 
			//log(touch.identifier);
			
			//this.touches[touch.identifier] = {
			//	touchStartTime : new Date().getTime(),
			//	touchStartX : touch.clientX, 
			//	touchStartY : touch.clientY, 
			//	stroked : false	}; 
			
			socket.send("p:catch|"+clock.getServerTime()); 
			
		}
		
	}
	
	function mouseDown (e) {
		log("mouseDown"); 
		socket.send("p:catch|"+clock.getServerTime()); 
		
		
	}
	
	
}
