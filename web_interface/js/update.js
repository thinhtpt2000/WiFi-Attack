function showPassword() {
    var x = document.getElementById("wifi-password");
    if (x.type === "password") {
        x.type = "text";
    } else {
        x.type = "password";
    }
}

function sendUpdate() {
    document.getElementById("error-msg").innerHTML = "";
    document.getElementById("submit").disabled = true;
    document.getElementById("submit").innerHTML = "Đang gửi...";
    var password = document.getElementById("wifi-password").value;
    if (validatePassword()) {
        showMessage("LOADING");
        getFile("run?cmd=update " + password, function () {
            setTimeout(draw, 15 * 1000);
        });
    } else {
        document.getElementById("error-msg").innerHTML = "Mật khẩu không đúng!";
        document.getElementById("submit").disabled = false;
        document.getElementById("submit").innerHTML = "Xác nhận";
    }
}

function validatePassword() {
    var password = document.getElementById("wifi-password").value;
    if (password && password.length >= 8) {
        return true;
    }
    return false;
}

function draw() {
    window.location.reload();
}