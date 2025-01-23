const container = document.querySelector(".container");
let buttonSquares = document.querySelector(".buttonHowManySquares");
let squaresPerSide = 16;

function getRandomColor() {
    var letters = '0123456789ABCDEF';
    var color = '#';
    for (var i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
  }

function drawGrid() {
    for (let i = 0; i < squaresPerSide; i++) {
        let lengthSquare = 400 / squaresPerSide;
        let lengthSquarePx = lengthSquare + "px";
        let myRow = document.createElement("div");
        myRow.id = "column" + i;
        container.appendChild(myRow);
        let Column = document.querySelector("#column" + i);
        for (let j = 0; j < squaresPerSide; j++) {
            let cell = document.createElement("div");
            cell.classList.add("cell");
            cell.style.width = lengthSquarePx;
            cell.style.height = lengthSquarePx;
            Column.appendChild(cell);
            cell.addEventListener('mouseover', () => {
                cell.style.backgroundColor = getRandomColor();
              });
        }
    }
}

drawGrid();

buttonSquares.addEventListener('click', () => {
    let nbSquares = prompt("How many squares per side do you want ? (Min is 1 and max is 100 !!)");
    if (!isNaN(nbSquares) && nbSquares > 0 && nbSquares <= 100)
    {
        squaresPerSide = nbSquares;
        while (container.hasChildNodes()) {
            container.removeChild(container.firstChild);
          }
        drawGrid();
    }
})