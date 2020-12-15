var customerName = prompt("Please enter your name", "<name goes here>");

if (customerName != null) {

	document.getElementById("welcome").innerHTML =

		"Hello " + customerName + "! How are you today?";

}
