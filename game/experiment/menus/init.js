// Intro effect when the page is loaded
document.addEventListener('DOMContentLoaded', () => {
    document.body.classList.add('loaded');
});

const pageElement = document.querySelector('#page');
const pageTitleElement = document.querySelector('#pageTitle');
const pageContentElement = document.querySelector('#pageContent');
const contentListElement = document.querySelector('#contentList');
const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

const registeredPages = new Map();
let currentPage = null;

const registeredMountableContent = []; // TODO: Fetch from menus.mountableContent

registeredMountableContent.push({
    icon: 'images/game-icons/garrysmod.png',
    name: 'Garry\'s Mod',
    id: 'garrysmod',
    mounted: true,
});
registeredMountableContent.push({
    icon: 'images/game-icons/cstrike.png',
    name: 'Counter-Strike: Source',
    id: 'cstrike',
    mounted: false,
});

registeredMountableContent.forEach((content, index) => {
    const contentItemElement = contentItemTemplateElement.content.firstElementChild.cloneNode(true);

    contentItemElement.classList.add(index % 2 === 0 ? 'bg-back/80' : 'bg-back/40');

    const iconElement = contentItemElement.querySelector('img');
    iconElement.src = content.icon;
    iconElement.alt = content.name;

    const nameElement = contentItemElement.querySelector('span');
    nameElement.textContent = content.name;

    const inputElement = contentItemElement.querySelector('input');
    inputElement.value = content.id;
    inputElement.checked = content.mounted;

    contentItemElement.addEventListener('click', () => {
        inputElement.checked = !inputElement.checked;
    });

    contentList.appendChild(contentItemElement);
});

// Custom element for links
customElements.define('game-menu-link', class extends HTMLElement {
    constructor() {
        super();

        this.addEventListener('click', this.onClick);

        this.classList.add('group', 'flex', 'gap-2', 'items-center', 'hover:scale-125', 'cursor-pointer', 'transition');

        if (this.hasAttribute('label-above')) {
            this.classList.add('flex-col-reverse');
        } else {
            this.classList.add('flex-col');
        }

        const key = this.getAttribute('translation');
        console.log(key); // TODO: request the menu for translations
        // this.textContent = menus.localize.getTranslation(key); // TODO: Set this on the span (not the element itself) or sub-links will break
    }

    connectedCallback() {
        const sizeClassImage = this.hasAttribute('small') ? 'w-12 h-12' : 'w-24 h-24';
        const sizeClassText = this.hasAttribute('small') ? 'text-sm' : 'text-md';
        this.innerHTML = `
            <img src="${this.getAttribute('icon')}"
                 alt="${this.textContent}"
                 class="${sizeClassImage}">
            <span class="scale-0 opacity-0 group-hover:opacity-100 group-hover:scale-100 transition-all ${sizeClassText}">${this.textContent}</span>
        `;
    }

    onClick(event) {
        event.preventDefault();

        const href = this.getAttribute('href');

        if (href) {
            if (currentPage) {
                currentPage.hide();
            }

            window.location.href = href;
            return;
        }

        const pageRef = this.getAttribute('pageref');

        if (pageRef) {
            const registeredPage = registeredPages.get(pageRef);

            if (registeredPage.isShowing()) {
                registeredPage.hide();
                return;
            }

            if (currentPage) {
                currentPage.hide();
            }

            registeredPage.show();
            currentPage = registeredPage;
            return;
        }
    }
});

// Custom element for pages (invisible until selected)
customElements.define('game-page', class extends HTMLElement {
    _lastShowHandler = 0;
    _isShowing = false;

    constructor() {
        super();

        this.classList.add('hidden');
    }

    connectedCallback() {
        registeredPages.set(this.id, this);
    }

    show() {
        this._isShowing = true;
        this._lastShowHandler++;

        pageTitleElement.textContent = this.getAttribute('title');
        // pageContentElement.innerHTML = this.innerHTML; // This messes up any values set (e.g: checked on checkboxes)
        // Lets teleport the children instead
        while (this.firstChild) {
            pageContentElement.appendChild(this.firstChild);
        }

        pageElement.classList.remove('opacity-0');
    }

    isShowing() {
        return this._isShowing;
    }

    hide() {
        this._isShowing = false;

        const showHandler = this._lastShowHandler;

        pageElement.classList.add('opacity-0');

        // Move the children back after the animation
        setTimeout(() => {
            // Don't move them if the show handler changed, meaning the page was shown again
            // (happens when spam clicking)
            if (showHandler !== this._lastShowHandler) {
                return;
            }

            while (pageContentElement.firstChild) {
                this.appendChild(pageContentElement.firstChild);
            }
        }, 500);
    }
});
