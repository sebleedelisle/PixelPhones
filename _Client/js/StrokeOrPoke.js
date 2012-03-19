
function StrokeOrPoke() {
	this.img = new Image()
	this.touches = {};
	this.purrSound = new Audio("strokeorpoke/purr.wav");
	this.meowSound = new Audio("strokeorpoke/meow1.wav"); 
	this.audioCheckInterval; 
	
	this.start = function() {
		
		this.img.onload = this.imageLoaded(); 
		
		this.img.src = "strokeorpoke/cat"+Math.floor(Math.random()*8)+".jpg";
		this.img.className = "fullScreenImage";
		changeColour("000000");
		
	//	this.img.style.webkitTransform = "scale("+(SCREEN_WIDTH/414)+","+(SCREEN_WIDTH/414)+")"; 
			
		//this.purrSound.load();
		//this.purrSound.play(); 
		//this.meowSound.load();
		//this.meowSound.play();
		
	};
	
	this.imageLoaded = function() { 
		
		document.body.appendChild(this.img);
		var that = this; 
		this.img.addEventListener("touchstart", function(e){ that.touchStart.call(that,e)});
		this.img.addEventListener("touchmove", function(e){ that.touchMove.call(that,e)});
		this.img.addEventListener("touchend", function(e){ that.touchEnd.call(that,e)});
		
		this.audioCheckInterval = setInterval(function(e){ that.checkAudio.call(that)}, 1);
	
	};
	
	this.touchStart = function (e) { 
		e.preventDefault(); 
		
		for (var i=0;i<e.changedTouches.length;i++) { 
			var touch = e.changedTouches[i]; 
			console.log(touch.identifier);
			//if(!this.touches[touch.identifier]) 
			this.touches[touch.identifier] = {
				touchStartTime : new Date().getTime(),
				touchStartX : touch.clientX, 
				touchStartY : touch.clientY, 
				stroked : false	}; 
			
		}
		
	};
	
	
	this.touchMove = function (e) { 
		e.preventDefault(); 
	
	//s	console.log(e.changedTouches.length);
		for (var i=0;i<e.changedTouches.length;i++) { 
			var touch = e.changedTouches[i]; 
			if(this.touches[touch.identifier]) {
			
				var mytouch = this.touches[touch.identifier];
				if(!mytouch.stroked) { 
					var touchLength = new Date().getTime() - mytouch.touchStartTime; 
					var dx =  touch.clientX - mytouch.touchStartX;
					var dy =  touch.clientY - mytouch.touchStartY;
					var touchDistance = Math.sqrt((dx*dx) + (dy*dy));
				
					if((touchDistance > 50) || (touchLength>200) ){
					
						this.stroke();
					
						
						mytouch.stroked = true;
					}
				}
				
			}
		}
		
	};		
	this.touchEnd = function (e) { 
		e.preventDefault(); 
	
	//	console.log(e.changedTouches.length);
		for (var i=0;i<e.changedTouches.length;i++) { 
			var touch = e.changedTouches[i]; 
			if(this.touches[touch.identifier]) {
				
				var mytouch = this.touches[touch.identifier];
				
				if(!mytouch.stroked) {
					
					this.poke(); 		
				}
				
				delete this.touches[touch.identifier];
			
			}
		}
		
	}
	
	
	this.stroke = function() { 
		
		//this.purrSound.volume = 1; 
		try {
			this.purrSound.pause();
			this.purrSound.currentTime = 0;
			this.purrSound.play();
		//	console.log("interupted purr", this.purrSound.currentTime);
		}catch(e) {
			this.purrSound.play();
		//	console.log("not interupted meow", this.purrSound.currentTime);
		};
		
		if(socket.readyState==1) {
			socket.send("p:stroke|"+clock.getServerTime()); 
		} 
		console.log("stroke", this); 
	};
	this.poke = function() {
		try { 
			this.meowSound.pause();
			this.meowSound.currentTime = 0;
			this.meowSound.play(); 
		//	console.log("interupted meow", this.meowSound.currentTime);
		} catch(e) {
			this.meowSound.play(); 
		//	console.log("not interupted meow", this.meowSound.currentTime);
		};
		
		if(socket.readyState==1) {
			socket.send("p:poke|"+clock.getServerTime()); 
		}		
		
		console.log("poke", this);
	};
	
	this.stop = function() { 
		this.removeKitten(); 
		clearInterval(this.audioCheckInterval); 
		this.audioCheckInterval = -1; 
	};
	
	this.removeKitten = function() { 			
		if(this.img.parentNode == document.body)
			document.body.removeChild(this.img);
				
	};
	this.checkAudio = function() { 
		try {
			if(this.purrSound.currentTime>3.0) {
				this.purrSound.pause();
				this.purrSound.currentTime = 0; 
			}
			if(this.meowSound.currentTime> 0.5) { 
				this.meowSound.pause();
				this.meowSound.currentTime = 0;
			}
		} catch(e) {}; 
		
	};
	
	this.receiveMessage = function(msg) { 
		
		console.log(msg); 
		
		if(msg.indexOf("flash")>-1){
			console.log("flashing!"); 
			this.removeKitten(); 
			changeColour("ffffff"); 
			
		}
		
	};
	
}
