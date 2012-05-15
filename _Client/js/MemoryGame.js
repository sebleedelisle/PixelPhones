function MemoryGame() { 
	
	var loopInterval, 
		counter,
		canvas = this.canvas = document.createElement('canvas'),
		ctx = this.ctx = canvas.getContext('2d'), 
		colours = ['red', 'yellow', 'green', 'blue'], 
		pressedColours = ['darkred', '#999900', 'darkgreen', 'darkblue'], 
		startShowingButtons = 0, 
		stopShowingButtons = 0, 
		buttonsVisible = false, 
		inPlay = true, 
		failImage, 
		failImageVisible = false, 
		winImage, 
		winImageVisible = false, 
		audioLoaded = false,
		soundSprite, 
		currentSound = -1;
		
	
		canvas.addEventListener("touchstart", touchStart);
		canvas.addEventListener("mousedown", mouseDown);
		canvas.addEventListener("touchend", touchEnd);
		canvas.addEventListener("mouseup", touchEnd);
	
	
	this.start = function() { 
		log("start memory game!"); 
		
		this.reset(); 
		
		loopInterval = setInterval(update, 1000/60); 
		changeColour("000066"); 
		loadSounds(); 
		
		
	};
	
	this.reset = function() {
	
		counter = 0; 
		changeColour("0000ff"); 
		inPlay = true; 
		hideButtons(); 
		hideFailImage(); 
		hideWinImage(); 
	
	}
	
	function update () {
		
		counter++; 

		// if we're in the press button phase we want to disable touches
		// using touchesDisabled = false
		var time = clock.getServerTime(); 
		
		if(inPlay) { 
			if((!buttonsVisible) && (time>startShowingButtons) && ( time<stopShowingButtons)) {
				showButtons(); 
				changeColour('000044'); 
				touchesDisabled = true; 
			}
		} else { 
			
			if((buttonsVisible) && (time>stopShowingButtons)) {
				hideButtons(); 
			
			}
			
			touchesDisabled = false; 			
			
			
		}
	}
	
	
		this.showButtons = function showButtons() { 

		canvas.width = SCREEN_WIDTH; 
		canvas.height = SCREEN_HEIGHT; 
		canvas.style.position = 'absolute'; 
		canvas.style.left = '0px';
		canvas.style.top = '0px';
	
		if(canvas.parentNode != document.body) document.body.appendChild(canvas); 
		
		drawButtons(); 
		
		buttonsVisible = true; 
	}
	
	function showFailImage() { 
		
	//	if(buttonsVisible) hideButtons(); 
		log('showFailImage');
		if(!failImage) { 
			log('creating failImage'); 
			failImage = this.failImage = document.createElement('canvas'); 
			failImage.width = SCREEN_WIDTH; 
			failImage.height= SCREEN_HEIGHT; 
			
			var c = failImage.getContext('2d'); 
			c.clearRect(0,0,failImage.width, failImage.height); 
			c.beginPath(); 
			c.moveTo(20,20); 
			c.lineTo(failImage.width-20, failImage.height-20); 
			c.moveTo(failImage.width-20, 20); 
			c.lineTo(20, failImage.height-20); 
			
			c.lineCap = 'round'; 
			c.lineWidth = 25; 
			c.strokeStyle = 'black'; 
			c.stroke(); 
			c.lineWidth = 20; 
			c.strokeStyle = 'red'; 
			c.stroke(); 
			
		
			failImage.style.position = 'absolute'; 
			failImage.style.left = '0px';
			failImage.style.top = '0px';
			failImage.style.pointerEvents = 'none';
			
			
		}
		
		if(failImage.parentNode != document.body) document.body.appendChild(failImage); 
		failImageVisible = true; 
		
		
	}
	
	function hideFailImage() { 
		
		if((failImage) && (failImage.parentNode == document.body)) document.body.removeChild(failImage); 
		failImageVisible = false; 
		
		
	}
	
	
	function showWinImage() { 
		
//		if(buttonsVisible) hideButtons(); 
		log('showWinImage');
		if(!winImage) { 
			log('creating failImage'); 
			winImage = this.failImage = document.createElement('canvas'); 
			winImage.width = SCREEN_WIDTH; 
			winImage.height= SCREEN_HEIGHT; 
			
			var c = winImage.getContext('2d'); 
			//c.fillStyle = 'black'; 
			c.clearRect(0,0,winImage.width, winImage.height); 
			c.beginPath(); 
			c.moveTo(winImage.width/4, winImage.height/2);
			c.lineTo(winImage.width/2, winImage.height/4*3);
			c.lineTo(winImage.width/4*3, winImage.height/4);
			c.lineCap = 'round'; 
			c.lineWidth = 25; 
			c.strokeStyle = 'black'; 
			c.stroke(); 
			c.lineWidth = 20; 
			c.strokeStyle = 'green'; 
			c.stroke(); 
		
			winImage.style.position = 'absolute'; 
			winImage.style.left = '0px';
			winImage.style.top = '0px';
			winImage.style.pointerEvents = 'none';
			
		}
		
		if(winImage.parentNode != document.body) document.body.appendChild(winImage); 
		winImageVisible = true; 
		
		
	}
	
	function hideWinImage() { 
		
		if((winImage) && (winImage.parentNode == document.body)) document.body.removeChild(winImage); 
		winImageVisible = false; 
		
		
	}
	
	
	
	
	function hideButtons() { 
		
		if(canvas.parentNode == document.body) document.body.removeChild(canvas); 
		buttonsVisible = false; 
		
	}
	
	function drawButtons(touchedButton) { 
		
		for(var i = 0; i<4;i++) { 
			ctx.fillStyle = (i==touchedButton)? pressedColours[i] :  colours[i]; 
			
			ctx.fillRect(((i%2) * SCREEN_WIDTH/2),(Math.floor(i/2) * SCREEN_HEIGHT/2),SCREEN_WIDTH/2, SCREEN_HEIGHT/2); 
		}			
	}
	
	
	this.stop = function() { 
		
		clearInterval(loopInterval); 
		hideButtons(); 
		hideWinImage(); 
		hideFailImage(); 
		changeColour("000000"); 
	};
	
	
	
	this.receiveMessage = function(msg) { 
		
		log(msg); 
		
		if(msg.indexOf("reset")>-1) { 
			this.reset(); 
		} else if(msg.indexOf('showButtons')>-1) { 
			
			var values = msg.split('|'); 
			log(values); 
			//showButtons(); 
			hideButtons(); 
			hideWinImage(); 
			hideFailImage();
			startShowingButtons = values[2]; 
			stopShowingButtons = values[3]; 
			inPlay = true;
			
		} else if(msg.indexOf('fail')>-1) { 
			inPlay = false; 
			showFailImage(); 
			//failSound.play(); 
			playSound(4); 
		}	else if(msg.indexOf('win')>-1) { 
				
			inPlay = false; 
			showWinImage(); 
		}
		
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
			var id = getIDFromTouchPos(touch.clientX, touch.clientY);
			socket.send('p:mem|press|'+id+'|'+clock.getServerTime()); 
		//	log('p:mem|press'+id+'|t'+clock.getServerTime());
			touchedButton = id; 
			drawButtons(id); 
			
			playSound(id); 
		}
		
	}
	
	function mouseDown (e) {
		log("mouseDown"); 
	
		var id = getIDFromTouchPos(e.clientX, e.clientY);
		socket.send('p:mem|press|'+id+'|'+clock.getServerTime()); 
		touchedButton = id; 
		drawButtons(id); 
		playSound(id); 
	//	log('p:mem|press'+id+'|t'+clock.getServerTime());
		
	}
	
	function touchEnd(e) { 
		touchedButton = -1; 
		drawButtons(); 
	}
	
	function getIDFromTouchPos(x, y) { 
		
		var id = Math.floor(x / (SCREEN_WIDTH/2));
		id+= Math.floor(y / (SCREEN_HEIGHT/2)) * 2; 
		return id; 
		
	}
	
	function loadSounds() { 
		
			
		soundSprite = new Audio("mem/SoundSpriteLow.wav");	
		// sounds[1] = new Audio("mem/Tone2low.wav");
		// 	sounds[2] = new Audio("mem/Tone3low.wav");
		// 	sounds[3] = new Audio("mem/Tone4low.wav");
		// 	
		// 	failSound = new Audio("mem/ToneFailLow.wav");
		// 	// burstSound.load();
		// 	// you have to play the sound to make sure it's loaded, otherwise you get 
		// 	// a glitch the first time you play it
		// 	burstSound.volume = 0; 
		// 	burstSound.play(); 
		// 	
	}
	function playSound(id) { 
		 if(!audioLoaded) { 
			soundSprite.play(); 

			audioLoaded = true; 
			
			var interval = setInterval(function() { 
					try{
						if((currentSound<0) || (soundSprite.currentTime>currentSound+0.1)) {
							soundSprite.pause();
							//soundSprite.volume = 0; 
							//soundSprite.play(); 
							currentSound = -1; 
						}
					} catch(e) { 
						//?
					}
				}, 100); 		
					
		} else {
	
			try { 
				soundSprite.pause();
				soundSprite.currentTime = id; 

				soundSprite.play(); 
				currentSound = id; 
			} catch(e) { 
				// ? 
			}
			
		}
		//sounds[0].play(); 
		//sounds[id].volume = 1; 
		
	}
}
