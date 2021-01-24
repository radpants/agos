import h2d.col.Point;

class Hex {
	var x:Int;
	var y:Int;
	var z:Int;
	var q:Int;
	var r:Int; 
	var neighbors:Array<Hex>;

	private static var NE = Hex.fromCube(1,-1,0);
	private static var E = Hex.fromCube(1,0,-1);
	private static var SE = Hex.fromCube(0,1,-1);
	private static var SW = Hex.fromCube(-1,1,0);
	private static var W = Hex.fromCube(-1,0,1);
	private static var NW = Hex.fromCube(0,-1,1);
	private static var NeighborOffsets:Array<Hex> = [NE, E, SE, SW, W, NW];
	private static var Sqrt3 = Math.sqrt(3.0);

	public function new(q:Int, r:Int) {
		this.q = q;
		this.r = r;
		this.x = Math.floor(q - (r + (r&1)) / 2);
		this.z = r;
		this.y = -x-z; 
	}

	public static function fromCube(x:Int, y:Int, z:Int) {
		var q:Int = Math.floor(x + (z + (z&1)) / 2);
		var r:Int = z;
		return new Hex(q,r);
	}

	public function findNeighbors(hexes:Array<Hex>, mapWidth:Int):Void { 
		neighbors = [];
		for(o in NeighborOffsets) { 
			var h:Hex = Hex.fromCube(x + o.x, y + o.y, z + o.z);
			var index:Int = h.r * mapWidth + h.q;
			if(index > 0 && index < hexes.length) 
				neighbors.push(h);
			else
				neighbors.push(null);
		}
	}

	public function getPosition():Point {
		var p = new Point();
		p.x = Sqrt3 * (q - 0.5 * (r&1));
		p.y = 3 / 2 * r;
		return p;
	}

}