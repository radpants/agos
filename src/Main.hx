import hxd.res.DefaultFont;
import h2d.Text;

class Main extends hxd.App {
	public override function init() {
		var label = new Text(DefaultFont.get(), s2d);
		label.text = "Testing";
		label.setPosition(32, 32); 
	}

	static function main() {
		new Main();
	}
}