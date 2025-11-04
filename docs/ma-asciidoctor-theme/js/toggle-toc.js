// hide / show the Table of Contents (TOC)
function toggleTOC() {
  var toc = document.getElementById("toc");
  var body = document.getElementsByTagName("body")[0];

  if (toc.style.display === "none") {
      toc.style.display = "block";
      body.classList.remove("toc-hidden");
  } else {
      toc.style.display = "none";
      body.classList.add("toc-hidden");
  }
}
