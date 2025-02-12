function setMethod(method) {
	document.getElementById("form").method = method;
}

function setMethod2(method) {
	document.getElementById("form2").method = method;
}

function deleteFile() {
	const filePath = document.getElementById("filename").value;
	if (!filePath)
		return alert("Please complete the required field!");

	fetch(filePath, {
		method: "DELETE"
	})
	.then((response) => {
		if (response.redirected)
			return window.location.href = response.url;
		if (response.status === 204)
			return alert("File deleted!");
		if (response.status === 404)
			return alert("Failed to delete file!");
		return response.text().then((html) => {
			document.open();
			document.write(html);
			document.close();
		});
	});
}
