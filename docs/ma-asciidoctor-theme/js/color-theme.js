window.addEventListener('DOMContentLoaded', () => {

    var theme = localStorage.getItem('theme');

    if (theme != 'light' && theme != 'dark') {
        theme = window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
        localStorage.setItem('theme', theme);
    }

    document.documentElement.setAttribute('data-ma-theme', theme);

    updateTheme(theme);
})

window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', event => {
    const newColorScheme = event.matches ? "dark" : "light";
    updateTheme(newColorScheme);
});

function updatePictureSrc(picture, theme) {

    const sources = picture.querySelectorAll('source');

    sources.forEach(source => {
        
        const media = source.getAttribute('media');

        dataTheme = source.getAttribute('data-theme');

        if (!dataTheme && media) {
            if (media.includes('prefers-color-scheme: dark')) {
                source.setAttribute('data-theme', 'dark');
                source.setAttribute('media', 'not all');
            } else if (media.includes('prefers-color-scheme: light')) {
                source.setAttribute('data-theme', 'light');
                source.setAttribute('media', 'not all');
            }
        }

        dataTheme = source.getAttribute('data-theme');

        if (dataTheme) {
            if (dataTheme === theme) {
                source.setAttribute('media', '(prefers-color-scheme: light),(prefers-color-scheme: dark)');
            } else {
                source.setAttribute('media', 'not all');
            }
        }
    });
}

function updateTheme(newTheme) {

    localStorage.setItem('theme', newTheme);

    document.documentElement.setAttribute('data-ma-theme', newTheme);

    document.querySelectorAll('picture').forEach(picture => {
        updatePictureSrc(picture, newTheme);
    });

    const cssUrl = 'https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.18.3/styles/';

    document.querySelectorAll('link[rel="stylesheet"][href]').forEach(link => {
        if (link.href.startsWith(cssUrl)) {
            link.href = cssUrl + (newTheme === 'dark' ? 'dark' : 'github') + '.min.css';
        }
    });
}

function toggleTheme() {

    var currentTheme = document.documentElement.getAttribute('data-ma-theme');

    var newTheme = currentTheme == 'dark' ? 'light' : 'dark';

    updateTheme(newTheme);
}
