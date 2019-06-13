

var soloTechStorage = localStorage;
document.addEventListener("click", function(){
	console.log("Hello world!");
});	

var historico = JSON.parse(soloTechStorage.getItem("historico"));
if (historico == null)
	historico = [];

var equipe= "Adriana Karla;\nBruno Lopes;\nChristian Rodrigues;\nElilson Leal;\nFelipe do Carmo;\nJulyson Batista;\nKeneson Liniker;\nLuciano Cabral;\nLutizemberg Oliveira;\nWaldeck Belo;";

let orientadoresElement = document.getElementById("orientadores");
let equipeElement = document.getElementById("equipe");
let mensagemElement = document.getElementById("mensagem");
let yl69dataElement = document.getElementById("yl69data");

// ajaxMain();
setarDadosOrientadores(orientadoresElement);
informarSePlantaPrecisaAgua(mensagemElement,yl69dataElement);
setarDadosEquipe(equipeElement);
adicionarEventos(equipeElement);

// function ajaxMain() {
	// var r = new XMLHttpRequest(); r.open("GET", "http://192.168.0.102/solotech/resources/main.html", true); 
	// r.onreadystatechange = function () { 
	// if (r.readyState != 4 || r.status != 200) 
		// return; 

		// var currentInterval = setInterval(()=>{
			// let conteudoElement = document.getElementById("conteudo");
			// conteudoElement.innerHTML = r.responseText;
			// clearInterval(currentInterval);
		// },500);
		// console.log("Response: ",r.responseText);
	// }; 

	// r.send();
// }

function setarDadosOrientadores(orientadoresElement){
	orientadoresElement.innerText="Prof. MSc. RONEI NUNES RIBEIRO; Prof. Esp. ODAIR CRISTIANO ABREU DA SILVA; "
}
function informarSePlantaPrecisaAgua(mensagemEl,yl69dataEl){
	let msg = null;
	if (yl69dataElement.value < 750)
		msg = "A planta não precisa de água.";
	else
		msg = "A planta precisa de água.";
	mensagemElement.innerText = msg;
	let registro = [new Date().toLocaleString("pt-BR",{timeZone:'America/Manaus'}+";"), msg+" ("+yl69dataElement.value+");"];
	gravarNoHistorico(registro);
}
function gravarNoHistorico(registro){
	historico.push(registro);
	soloTechStorage.setItem("historico", JSON.stringify(historico));
	console.log("Historico: ",historico);
}
function setarDadosEquipe(equipeElement){
	equipeElement.value = equipe;
	equipeElement.style.height = (equipeElement.scrollHeight)+"px";
}
function adicionarEventos(equipeElement){
	equipeElement.addEventListener("click", function(){
		alert("Equipe: \n"+equipe.replace(new RegExp(";",'g')," "));

		let strHist = "";
		for (let i=0;i<historico.length;i++){
			strHist+=historico[i][0].concat(": ").concat(historico[i][1]);
			strHist+="\n";
		}
		alert(strHist);
	});
}