const BACKGROUND_INITIAL = 0;
const BACKGROUND_LOADING = 1;
const BACKGROUND_MAINMENU = 2;
const BACKGROUND_LEVEL = 3;
const BACKGROUND_DISCONNECTED = 4;

function SetBackgroundRenderState(state) {
    if (state === BACKGROUND_LEVEL) {
        document.body.classList.add('in-level');
    } else {
        document.body.classList.remove('in-level');
    }
}

const pageElement = document.querySelector('#page');
const pageTitleElement = document.querySelector('#pageTitle');
const pageCloseElement = document.querySelector('#pageClose');
const pageContentElement = document.querySelector('#pageContent');
const contentListElement = document.querySelector('#contentList');
const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

const registeredPages = new Map();
let currentPage = null;

pageCloseElement.addEventListener('click', () => {
    if (currentPage) {
        currentPage.hide();
        currentPage = null;
    }
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

// Source: https://codepen.io/marcusparsons/pen/NMyzgR
function makeDraggable(element) {
    let currentPosX = 0, currentPosY = 0, previousPosX = 0, previousPosY = 0;
    element.querySelector('[x-draggable-handle]').onmousedown = dragMouseDown;

    function dragMouseDown(e) {
        e.preventDefault();
        // Get the mouse cursor position and set the initial previous positions to begin
        previousPosX = e.clientX;
        previousPosY = e.clientY;
        // When the mouse is let go, call the closing event
        document.onmouseup = closeDragElement;
        // call a function whenever the cursor moves
        document.onmousemove = elementDrag;
    }

    function elementDrag(e) {
        e.preventDefault();
        // Calculate the new cursor position by using the previous x and y positions of the mouse
        currentPosX = previousPosX - e.clientX;
        currentPosY = previousPosY - e.clientY;
        // Replace the previous positions with the new x and y positions of the mouse
        previousPosX = e.clientX;
        previousPosY = e.clientY;
        // Set the element's new position
        element.style.top = (element.offsetTop - currentPosY) + 'px';
        element.style.left = (element.offsetLeft - currentPosX) + 'px';
    }

    function closeDragElement() {
        document.onmouseup = null;
        document.onmousemove = null;
    }
}

document.querySelectorAll('[x-draggable]').forEach(makeDraggable);

function initialize() {
    GameUI.LoadMountableContentInfo(function (registeredMountableContent) {
        registeredMountableContent.forEach((content, index) => {
            const contentItemElement = contentItemTemplateElement.content.firstElementChild.cloneNode(true);

            contentItemElement.classList.add(index % 2 === 0 ? 'bg-white/10' : 'bg-white/5');

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
    });
    console.log(navigator.userAgent);
}

window.addEventListener('interop:ready', () => {
    // Intro effect when the page is loaded
    document.body.classList.add('loaded');

    initialize();
});

// When working in the browser we want to mock the GameUI API
window.addEventListener('interop:installmock', () => {
    window.GameUI = {
        LoadMountableContentInfo: function (callback) {
            callback([
                {
                    id: 'gmod',
                    name: 'Garry\'s Mod',
                    mounted: true,
                    icon: './images/game-icons/garrysmod.png',
                },
                {
                    id: 'cstrike',
                    name: 'Counter-Strike: Source',
                    mounted: false,
                    icon: './images/game-icons/cstrike.png',
                },
                {
                    id: 'hl2',
                    name: 'Dev Note: this mountable game data is mocked',
                    mounted: false,
                    icon: './images/game-icons/hl2.png',
                },
            ]);
        },
    };
});
