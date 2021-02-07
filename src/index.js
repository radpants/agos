
const ready = (f) => {
	if(document.readyState !== 'loading') f(null);
	else document.addEventListener('DOMContentLoaded', (e) => {
		document.removeEventListener('DOMContentLoaded', f);
		f(e);
	});
};

const loader = PIXI.Loader.shared;
loader.add('isoshader', 'data/isoshader.glsl');
loader.add('kitten', 'data/images/TestKitten.png');

loader.load((loader, r) => {
	ready((e) => {
		const app = new PIXI.Application({
			width: window.innerWidth,
			height: window.innerHeight,
			antialias: true,
			transparent: false,
			resolution: 1, // TODO: Check for dpi scaling
			sharedTicker: true,
			backgroundColor: 0x202428
		});

		const filter = new PIXI.Filter(null, r.isoshader.data, { 
			uResolution: {
				x: parseFloat(app.renderer.width), 
				y: parseFloat(app.renderer.height) 
			},
			test: { x: 0.5, y: 0.5 } 
		});

		const sprite = new PIXI.Sprite();
		sprite.width = app.renderer.width;
		sprite.height = app.renderer.height;
		sprite.filters = [filter];

		const renderTexture = PIXI.RenderTexture.create(sprite.width, sprite.height); 

		const rtSprite = new PIXI.Sprite(renderTexture);

		app.stage.addChild(rtSprite); 

		let firstFrame = true;
		let t = 0.0;

		app.ticker.add(() => {
			if(firstFrame === true) {
				app.renderer.render(sprite, renderTexture);
				sprite.filters = [];
				firstFrame = false;
			}

			rtSprite.x = 50.0 * Math.cos(t);
			rtSprite.y = 50.0 * Math.sin(t);
			t += 0.03; 

			//sprite.cacheAsBitmap = true;
			//sprite.filters = [];
		});

		document.body.appendChild(app.view); 

		const root = document.getElementById('ui');

		m.render(root, m('p', 'Hello there!')); 
	}); 
});



