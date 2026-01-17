
const nodeWidth = 50;
const nodeHeight = 70;
const nodeGap = 50;
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

const box = {
    x: 0,
    y: 0,
};

const tree = {
    root: "A",
    children: [
        {
            root: "B",
            children: [{root:"C", children:[]}, {root:"D", children:[]}, {root:"E", children:[]}],
        },
        {
            root: "F",
            children: []
        },
        {root: "H", children:[{root:"G",children:[]}]},
        {root: "I", children:[]}
    ],
};

function _getBoundingBox(tree) {
    if (tree.children.length == 0) {
        return {width: nodeWidth, height: nodeHeight};
    }
    bbChildren = tree.children.map(getBoundingBox);

    var childrenSpan = bbChildren[0].width;
    var childrenMaxHeight = bbChildren[0].height;

    for (var i = 1; i < bbChildren.length; ++i) {
        childrenSpan += nodeGap + bbChildren[i].width;
        childrenMaxHeight = Math.max(childrenMaxHeight, bbChildren[i].height);
    }

    return {width: childrenSpan, height: childrenMaxHeight+nodeGap+nodeWidth};
}

function getBoundingBox(tree) {

    if (!tree._bb) {
        tree._bb = _getBoundingBox(tree);
    }
    return tree._bb;
}

let draggingBox = false;

function setSize() {
    canvas.height = innerHeight;
    canvas.width = innerWidth;
    draw();
}

function setCanvasStyle() {
    canvas.style.position = "absolute";
    canvas.style.left = 0;
    canvas.style.top = 0;
    canvas.style.border = "1px solid black";
}

function clip(x, low, high) {
    return Math.min(Math.max(low, x), high);
}

function updateMovement(e) {
    box.x += e.movementX;
    box.y += e.movementY;
}

function handleMouseDown(e) {
    draggingBox = e.clientX >= box.x-nodeWidth/2 && e.clientX <= box.x+nodeWidth/2 &&
        e.clientY >= box.y-nodeHeight/2 && e.clientY <= box.y + nodeHeight/2;
}

function handleMouseMove(e) {
    if (draggingBox) {
        updateMovement(e);
        draw();
    }
}

function handleMouseUp(e) {
    if (draggingBox) {
        updateMovement(e);
        draggingBox = false;
        draw();
    }
}

function setup() {
    addEventListener("resize", setSize);
    addEventListener("mousedown", handleMouseDown);
    addEventListener("mouseup", handleMouseUp);
    addEventListener("mousemove", handleMouseMove);
    setCanvasStyle();
    setSize();
}

function drawTree(tree, position) {
    console.log(`Drawing ${tree.root} at (${position.x}, ${position.y})`);
    ctx.strokeRect(position.x-nodeWidth/2, position.y-nodeHeight/2, nodeWidth, nodeHeight);
    ctx.font = "48px serif";
    const textMetrics = ctx.measureText(tree.root);
    ctx.fillText(tree.root, position.x - textMetrics.width/2, position.y + (textMetrics.actualBoundingBoxAscent-textMetrics.actualBoundingBoxDescent)/2);
    var bb = getBoundingBox(tree);
    //ctx.strokeRect(position.x-bb.width/2, position.y-nodeHeight/2, bb.width, bb.height);
    var positionChild = {x: position.x - bb.width/2, y: position.y + nodeHeight + nodeGap};

    for (var child of tree.children) {
        positionChild.x += child._bb.width/2;
        drawTree(child, positionChild);
        ctx.beginPath();
        console.log(`line from (${position.x}, ${position.y+nodeHeight/2}) to (${positionChild.x}, ${positionChild.y-nodeHeight/2})`);
        ctx.moveTo(position.x, position.y+nodeHeight/2);
        ctx.lineTo(positionChild.x, positionChild.y-nodeHeight/2);
        ctx.closePath();
        ctx.stroke();
        positionChild.x += child._bb.width/2 + nodeGap;

    }
}

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawTree(tree, {x: box.x, y: box.y});
    //ctx.fillRect(box.x, box.y, box.width, box.height);
}

setup();
draw();

