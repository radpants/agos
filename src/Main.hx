import h3d.shader.Parallax;
import h3d.col.Plane;
import h3d.col.Ray;
import hxd.Window;
import h3d.col.Point;
import h2d.col.Point in Point2D;
import h3d.prim.Polygon;
import h3d.scene.Mesh;
import hxd.res.DefaultFont;
import h2d.Text;
import h3d.Vector;

class Main extends hxd.App {

	var hexes:Array<Hex> = new Array<Hex>();
	var isDraggingMap:Bool = false; 
	var previousMousePos = new Point2D();
	var label:Text;

	static private var GroundPlane = Plane.fromNormalPoint(new Point(0,0,1), new Point());

	private function onEvent(event:hxd.Event):Void {
		switch(event.kind) {
			case EMove:
				if(isDraggingMap) {
					var ray = s3d.camera.rayFromScreen(event.relX, event.relY);
					var rayP = s3d.camera.rayFromScreen(previousMousePos.x, previousMousePos.y);
					var p = ray.intersect(GroundPlane);
					var pp = rayP.intersect(GroundPlane);
					if( p != null && pp != null ) {
						s3d.camera.target = s3d.camera.target.add(new Vector(pp.x - p.x, pp.y - p.y, 0.0)); 
						s3d.camera.pos = s3d.camera.pos.add(new Vector(pp.x - p.x, pp.y - p.y, 0.0)); 
						s3d.camera.update(); 
					}
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
			case _: 
		} 
	}

	public override function update(dt:Float) { 
		label.text = s3d.camera.target.toString();
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

		var hexSize = 0.25;
		var offset = new Point(-hexSize * mapSize / 2, -hexSize * mapSize / 2, 0);

		var geo = new h3d.prim.Cylinder(6, hexSize, 0.1, true);
		geo.addUVs();
		geo.addNormals(); 

		for(h in hexes) {
			var p = h.getPosition();
			var mesh = new Mesh(geo, s3d);
			mesh.material.color.setColor(0x4399ff);
			mesh.material.mainPass.enableLights = true; 
			mesh.setPosition(p.x * hexSize + offset.x, p.y * hexSize + offset.y, 0.2);
			mesh.setRotation(0, 0, Math.PI / 6);
		} 

		var idx = new hxd.IndexBuffer(6);
		idx.push(0); idx.push(1); idx.push(2);
		idx.push(2); idx.push(1); idx.push(3); 
		var groundGeo = new Polygon([
			new Point(-10.0, -10.0, 0.0),
			new Point(10.0, -10.0, 0.0),
			new Point(-10.0, 10.0, 0.0),
			new Point(10.0, 10.0, 0.0)
		], idx);
		groundGeo.addNormals();

		var ground = new Mesh(groundGeo, s3d); 
		ground.material.color.setColor(0xcccccc);
		ground.material.mainPass.enableLights = true;
		ground.material.receiveShadows = true;
		ground.material.castShadows = false;

		var sun = new h3d.scene.fwd.DirLight(new h3d.Vector(0.5, 0.15, -0.5), s3d);
		s3d.lightSystem.ambientLight.set(0.15, 0.2, 0.25); 

		s3d.camera.pos.set(0.0, 20.0, 20.0); 
	}

	static function main() {
		new Main();
	}
}