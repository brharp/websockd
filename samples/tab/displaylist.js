
class DisplayList extends Array
{
	beginPath() {
		this.push((cx) => cx.beginPath());
	}

	call(cx) {
		this.forEach((op) => op(cx));
	}

	clearRect(x, y, width, height) {
		this.push((cx) => cx.clearRect(x, y, width, height));
	}

	fillText(s, x, y) {
		this.push((cx) => cx.fillText(s, x, y));
	}

	lineTo(x, y) {
		this.push((cx) => cx.lineTo(x, y));
	}

	moveTo(x, y) {
		this.push((cx) => cx.moveTo(x, y));
	}

	restore() {
		this.push((cx) => cx.restore());
	}

	save() {
		this.push((cx) => cx.save());
	}

	set lineWidth(width) {
		this.push((cx) => cx.linewidth = width);
	}

	set strokeStyle(style) {
		this.push((cx) => cx.strokeStyle = style);
	}

	stroke() {
		this.push((cx) => cx.stroke());
	}

	strokeRect(x, y, width, height) {
		this.push((cx) => cx.strokeRect(x, y, width, height));
	}

	translate(x, y) {
		this.push((cx) => cx.translate(x, y));
	}
}

