import h2d.Object;
import h2d.Graphics;
import hxd.Window;
import h2d.col.Point;
import hxd.res.DefaultFont;
import h2d.Text;
import h3d.Engine; 

class Main extends hxd.App { 
	var hexes:Array<Hex> = new Array<Hex>();
	var isDraggingMap:Bool = false;
	var previousMousePos = new Point();
	var label:Text;

	private function onEvent(event:hxd.Event):Void {
		switch(event.kind) {
			case EMove:
				if(isDraggingMap) {
					s2d.camera.x += (previousMousePos.x - event.relX);
					s2d.camera.y += (previousMousePos.y - event.relY);
					previousMousePos.set(event.relX, event.relY);
				}
			case EPush:
				if(event.button == 0) {
					isDraggingMap = true;
					previousMousePos.set(event.relX, event.relY);
				}
			case ERelease:
				if(event.button == 0)
					isDraggingMap = false;
			case EKeyDown:

			case _: 
		} 
	}

	public override function update(dt:Float) { 
	}

	public override function init() { 

		// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
		// Data

		var mapSize = 32;

		for(y in 0...mapSize) {
			for(x in 0...mapSize) {
				var hex = new Hex(x,y);
				hexes.push(hex);
			}
		}

		for(h in hexes) {
			h.findNeighbors(hexes, mapSize);
		}

		var provinceSpacing = 6; 
		var provinceIndex = 0; 

		for(y in 0...mapSize/provinceSpacing) {
			for(x in 0...mapSize/provinceSpacing) { 
				var hex = hexes[(y*provinceSpacing) * mapSize + (x*provinceSpacing)]; 
				hex.province = provinceIndex;
				provinceIndex++;
			}
		} 

		var colors = []; 
		for(i in 0...1024) { colors.push(Std.random(0xffffff)); }

//		var unclaimedHexCount = hexes.length - provinceIndex;
//		while(unclaimedHexCount > 0) {
//			for(h in hexes) {
//				if(h.province != -1) {
//					for(n in h.neighbors) {
//						if(n.province == -1) {
//							n.province = h.province;
//							unclaimedHexCount--;
//							colors.push(Std.random(0xffffff));
//						}
//					}
//				}
//			}
//		}

		// -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
		// Rendering

		var window = Window.getInstance();
		window.title = "agos";
		window.resize(1280, 720);
		window.vsync = true;
		window.addEventTarget(onEvent);

		label = new Text(DefaultFont.get(), s2d);
		label.text = "Testing";
		label.setPosition(32, 32); 

		var hexSize = 15;
		var offset = new Point(-hexSize * mapSize / 2, -hexSize * mapSize / 2);

		var map = new Object(s2d); 

		for(h in hexes) {
			var p = h.getPosition();
			var gfx = new Graphics(map);
			gfx.smooth = true;
			gfx.lineStyle(1, 0x222222);
			gfx.drawCircle(0, 0, hexSize, 6);
			if(h.province != -1) {
				gfx.beginFill(colors[h.province%colors.length]);
				gfx.drawCircle(0, 0, hexSize-1, 6);
				gfx.endFill();
			}
			gfx.setPosition(p.x * hexSize + offset.x, p.y * hexSize + offset.y);
			gfx.rotation = Math.PI / 6;
		} 
	}

	static function main() {
		new Main();
	}
}
