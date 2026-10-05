
function getData()
{
	var lines;
	var objekt;
	var array;
	lines = datafile.split("<");
	for(var i = 0;i<lines.length-1;i++)
	{
		objekt = lines[i].split("|");
		display(objekt);
	}
}

function display(value)
{
	var appendto = document.getElementById("appendto");
	
	var tr = document.createElement("tr");
	tr.setAttribute("style","align:center;width:100%;");
	
	/* UID */
	var td = document.createElement("td");
	td.innerHTML = value[0];
	td.setAttribute("class","tablestyle");
	tr.appendChild(td);
	
	/* Verbrauch */
	td = document.createElement("td");
	td.innerHTML = value[1];
	td.setAttribute("class","tablestyle");
	tr.appendChild(td);
	
	/* Geladen von*/
	td = document.createElement("td");
	td.innerHTML = value[2];
	td.setAttribute("class","tablestyle");
	tr.appendChild(td);
	
	/* Geladen bis */
	td = document.createElement("td");
	td.innerHTML = value[3];
	td.setAttribute("class","tablestyle");
	tr.appendChild(td);
	
	appendto.appendChild(tr);
}
	