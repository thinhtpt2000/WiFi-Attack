var passwordJSON = [];

function load() {
    getFile("password.json", function (res) {
        passwordJSON = JSON.parse(res);
        drawPassword();
    });
}

function drawPassword() {
    var html;
    var date;

	// Access Points
	getE("passwordNum").innerHTML = passwordJSON.length;
	html = "<tr>"
		+ "<th class='id'></th>"
        + "<th class='mac'>MAC Address</th>"
		+ "<th class='ssid'>SSID</th>"
		+ "<th class='password'>Password</th>"
		+ "<th class='verify centered'>Verified</th>"
		+ "<th class='remove'></th>"
		+ "</tr>";

	for (var i = 0; i < passwordJSON.length; i++) {
        date = new Date(passwordJSON[i][3]);
		html += "<tr>"
			+ "<td class='id'>" + (i+1) + "</td>" // ID
            + "<td class='mac'>" + esc(passwordJSON[i][0]) + "</td>" // MAC
			+ "<td class='ssid'>" + esc(passwordJSON[i][1]) + "</td>" // SSID
			+ "<td class='password'>" +  esc(passwordJSON[i][2])  + "</td>" // Password
            + "<td class='verify centered'>" + (passwordJSON[i][3] ? "&#x2713;" : "") + "</td>" // Verify Emoji
			+ "<td class='remove'><button class='red' onclick='remove(" + i + ")'>X</button></td>" // Remove
			+ "</tr>";
	}

	getE("passwordTable").innerHTML = html;
}

function remove(id) {
    passwordJSON.splice(id, 1);
    drawPassword();
    getFile("run?cmd=remove password " + id);
}