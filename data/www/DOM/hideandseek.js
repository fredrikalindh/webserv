// Add your script code here
var titles = document.getElementsByTagName("h2");

const onClick = (evt) => {
  var headerClicked = evt.currentTarget;
  const div = headerClicked.nextElementSibling;
  const isCollapsed = headerClicked.firstChild.innerText[0] === "+";
  headerClicked.firstChild.innerText = isCollapsed ? "- " : "+ ";
  div.setAttribute("style", isCollapsed ? "" : "display: none");
  
};

for (var i = 0; i < titles.length; i++) {
  var title = titles[i];
  title.innerHTML = '<span class="mono">- </span>' + title.innerHTML;
  title.addEventListener("click", onClick, true);
  var relatedDiv = title.nextElementSibling;
  //   var collapseMark = title.firstChild;
  //   collapseMark.innerText = "+ ";
  //   relatedDiv.setAttribute("style", "display: none");
  var evt = document.createEvent("mouseEvent");
  evt.initEvent("click", true, false);
  title.dispatchEvent(evt);
}
