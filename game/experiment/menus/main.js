const BACKGROUND_CYCLE_INTERVAL_MS = 10000;

const BACKGROUND_INITIAL = 0;
const BACKGROUND_LOADING = 1;
const BACKGROUND_MAINMENU = 2;
const BACKGROUND_LEVEL = 3;
const BACKGROUND_DISCONNECTED = 4;

const backgrounds = ['outland.jpg', 'breens-office.jpg'];
let backgroundIndex = Math.floor(Math.random() * backgrounds.length);

const containerEl = document.getElementById('background-container');

const backgroundEl = document.createElement('div');
backgroundEl.classList.add('background');
backgroundEl.style.backgroundImage = `url('images/backgrounds/${backgrounds[backgroundIndex]}')`;

const backgroundBufferEl = document.createElement('div');
backgroundBufferEl.classList.add('background-next');

containerEl.append(backgroundEl, backgroundBufferEl);

function changeBackground() {
  backgroundIndex = (backgroundIndex + 1) % backgrounds.length;
  backgroundBufferEl.style.backgroundImage = `url('images/backgrounds/${backgrounds[backgroundIndex]}')`;
  backgroundBufferEl.style.opacity = '1';

  setTimeout(() => {
    backgroundEl.style.backgroundImage = backgroundBufferEl.style.backgroundImage;
    backgroundBufferEl.style.opacity = '0';
  }, 1000);
}

setInterval(changeBackground, BACKGROUND_CYCLE_INTERVAL_MS);

function SetBackgroundRenderState(state) {
  if (state === BACKGROUND_LEVEL) {
    document.body.classList.add('in-level');
  } else {
    document.body.classList.remove('in-level');
  }
}

const pageElement = document.querySelector('#page');
const pageTitleElement = document.querySelector('#pageTitle');
const pageContentElement = document.querySelector('#pageContent');
const contentListElement = document.querySelector('#contentList');
const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

const registeredPages = new Map();
let currentPage = null;

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

/**
 * Loads the mountable content info from the game, populating the list
 * and setting up the event listeners for mounting and unmounting content.
 */
function initialize() {
  GameUI.LoadMountableContentInfo(function (registeredMountableContent) {
    // sort it by name first
    registeredMountableContent.sort((a, b) => a.name.localeCompare(b.name));
    registeredMountableContent.forEach((content, index) => {
      const contentItemElement = contentItemTemplateElement.content.firstElementChild.cloneNode(true);

      contentItemElement.classList.add(index % 2 === 0 ? 'bg-white/10' : 'bg-white/5');

      if (index === 0) {
        contentItemElement.classList.add('origin-top-left');
      }

      const iconElement = contentItemElement.querySelector('img');
      iconElement.src = content.icon;
      iconElement.alt = content.name;

      const nameElement = contentItemElement.querySelector('span');
      nameElement.textContent = content.name;

      const inputElement = contentItemElement.querySelector('input');
      inputElement.value = content.id;
      inputElement.checked = content.mounted;

      inputElement.addEventListener('change', () => {
        if (inputElement.checked) {
          showLoadingOverlay('Mounting content...', function (finish) {
            GameUI.MountGameContent(inputElement.value, function (wasSuccessful) {
              if (!wasSuccessful) {
                inputElement.checked = false;
              }

              finish();
            });
          });
        } else {
          showLoadingOverlay('Unmounting content...', function (finish) {
            GameUI.UnmountGameContent(inputElement.value, function (wasSuccessful) {
              if (!wasSuccessful) {
                inputElement.checked = true;
              }

              finish();
            });
          });
        }
      });

      contentItemElement.addEventListener('click', (ev) => {
        // Don't toggle the checkbox if the click was on the checkbox itself
        // otherwise we immediately toggle it back
        if (ev.target.tagName === 'INPUT') {
          return;
        }

        inputElement.checked = !inputElement.checked;
        inputElement.dispatchEvent(new Event('change'));
      });

      contentList.appendChild(contentItemElement);
    });
  });
}

window.addEventListener('interop:ready', () => {
  // Intro effect when the page is loaded
  document.body.classList.add('loaded');

  initialize();
});

// When working in the browser we want to mock the GameUI API
window.addEventListener('interop:installmock', () => {
  window.GameUI = {
    MountGameContent: function (value, callback) {
      console.log(`Mocking mounting content ${value}`);
      setTimeout(() => {
        callback(true);
      }, 1000);
    },
    UnmountGameContent: function (value, callback) {
      console.log(`Mocking unmounting content ${value}`);
      setTimeout(() => {
        callback(true);
      }, 1000);
    },
    LoadMountableContentInfo: function (callback) {
      callback([
        {
          id: 4000,
          name: 'Garry\'s Mod',
          mounted: true,
          icon: './images/game-icons/garrysmod.png',
        },
        {
          id: 240,
          name: 'Counter-Strike: Source',
          mounted: false,
          icon: './images/game-icons/cstrike.png',
        },
        {
          id: 220,
          name: 'Half-Life 2',
          mounted: false,
          icon: './images/game-icons/hl2.png',
        },
        {
          id: 280,
          name: 'Half-Life',
          mounted: true,
          icon: './images/game-icons/hl1.png',
        },
        {
          id: 300,
          name: 'Day of Defeat: Source',
          mounted: false,
          icon: './images/game-icons/dod.png',
        },
        {
          id: 340,
          name: 'Half-Life 2: Lost Coast',
          mounted: false,
          icon: './images/game-icons/lostcoast.png',
        },
        {
          id: 320,
          name: 'Half-Life Deathmatch: Source',
          mounted: false,
          icon: './images/game-icons/hl1mp.png',
        },
        {
          id: 380,
          name: 'Half-Life 2: Episode One',
          mounted: false,
          icon: './images/game-icons/episodic.png',
        },
        {
          id: 400,
          name: 'Portal',
          mounted: false,
          icon: './images/game-icons/portal.png',
        },
        {
          id: 420,
          name: 'Half-Life 2: Episode Two',
          mounted: false,
          icon: './images/game-icons/ep2.png',
        },
        {
          id: 440,
          name: 'Team Fortress 2',
          mounted: false,
          icon: './images/game-icons/tf.png',
        },
      ]);
    },
  };
});
