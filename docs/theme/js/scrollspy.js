const toc = document.querySelector("#toc");

const sectionLinkNodes = toc.querySelectorAll(
  ".sectlevel1 > li > a, .sectlevel2 > li > a, .sectlevel3 > li > a, .sectlevel4 > li > a, .sectlevel5 > li > a"
);

var sectionLinks = {};

sectionLinkNodes.forEach(function(sectionLinkNode) {
    sectionLinks[sectionLinkNode.hash] = sectionLinkNode;
});

var sections = [];

sections = [
...document.querySelectorAll('.sect5'),
...document.querySelectorAll('.sect4'),
...document.querySelectorAll('.sect3'),
...document.querySelectorAll('.sect2'),
...document.querySelectorAll('.sect1'),
];

window.addEventListener('scroll', function () {

    const currentPos = window.scrollY + 80;

    for (var i = 0; i < sections.length; i++) {

        const section = sections[i];

        const sectionTop = section.offsetTop + section.offsetParent.offsetTop;
        const sectionHeight = section.offsetHeight;
        const sectionId = section.getAttribute('id');

        if (currentPos >= sectionTop && currentPos < sectionTop + sectionHeight) {

            sectionLinkNodes.forEach(function (navbarLink) {
                navbarLink.classList.remove('active');
            });

            const reference = '#' + section.children[0].id;

            if (reference in sectionLinks) {

                currentSectionLink = sectionLinks[reference];

                currentSectionLink.classList.add('active');

                currentSectionLink.scrollIntoView({
                    behavior: 'smooth',
                    block: 'nearest',
                    inline: 'nearest'
                });        

            } else {
                console.warn('Reference not found: ' + reference);
            }

            break;

        }
    }

});
