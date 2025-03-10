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

function ServerListComplete(wasSuccessful, errorMessage) {
  console.log('Server list complete:', wasSuccessful, errorMessage);
}

function ServerListFailed(errorMessage) {
  console.log('Server list failed:', errorMessage);
}

function ServerListAdd(serverInfo) {
  console.log('Server list add:', serverInfo);
}

const pageElement = document.querySelector('#page');
const pageTitleElement = document.querySelector('#pageTitle');
const pageContentElement = document.querySelector('#pageContent');
const contentListElement = document.querySelector('#contentList');
const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

const registeredPages = new Map();
let currentHideTimeout = null;
let currentPage = null;

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
    console.log(key);
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

      // Small delay to ensure the hide animation has started
      setTimeout(() => {
        registeredPage.show();
      }, 50);

      return;
    }
  }
});

// Custom element for pages (invisible until selected)
customElements.define('game-page', class extends HTMLElement {
  _isShowing = false;
  _contentHolder = null;
  _initialized = false;
  _initializeFunction = null;

  constructor() {
    super();
    this.classList.add('hidden');

    // Create a content holder div for this page
    this._contentHolder = document.createElement('div');
    this._contentHolder.classList.add('page-content-holder', 'h-full', 'flex', 'flex-col');
  }

  connectedCallback() {
    registeredPages.set(this.id, this);

    // Move all initial children to the content holder
    while (this.firstChild) {
      this._contentHolder.appendChild(this.firstChild);
    }
  }

  // Set an initialization function for this page
  setInitFunction(initFn) {
    this._initializeFunction = initFn;
  }

  show() {
    // Clear any pending hide operations
    if (currentHideTimeout) {
      clearTimeout(currentHideTimeout);
      currentHideTimeout = null;
    }

    this._isShowing = true;
    currentPage = this;

    pageTitleElement.textContent = this.getAttribute('title');

    // Clear current content
    while (pageContentElement.firstChild) {
      pageContentElement.removeChild(pageContentElement.firstChild);
    }

    // Append the content holder to the page content
    pageContentElement.appendChild(this._contentHolder);

    // Initialize the page if needed
    if (!this._initialized && this._initializeFunction) {
      this._initializeFunction();
      this._initialized = true;
    }

    // Make the page visible
    pageElement.classList.remove('opacity-0');
  }

  isShowing() {
    return this._isShowing;
  }

  hide() {
    this._isShowing = false;

    // Hide the page visually
    pageElement.classList.add('opacity-0');

    // Set timeout to move content back after transition
    currentHideTimeout = setTimeout(() => {
      if (pageContentElement.contains(this._contentHolder)) {
        pageContentElement.removeChild(this._contentHolder);
      }
      currentHideTimeout = null;
    }, 500);
  }
});

/**
 * Loads the mountable content info from the game, populating the list
 * and setting up the event listeners for mounting and unmounting content.
 */
function initialize() {
  GameUI.LoadMountableContentInfo(function (registeredMountableContent) {
    // Get the content page
    const contentPage = registeredPages.get('content');

    contentPage.setInitFunction(() => {
      const contentList = contentPage._contentHolder.querySelector('#contentList');
      const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

      if (!contentList) return;

      // Clear existing content
      contentList.innerHTML = '';

      // sort it by name first
      registeredMountableContent.sort((a, b) => a.name.localeCompare(b.name));
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
  });

  // Setup the host server page
  const hostServerPage = registeredPages.get('host-server');

  hostServerPage.setInitFunction(() => {
    // This function only runs when the host-server page is shown for the first time
    const pageContent = hostServerPage._contentHolder;
    const gameModeSelect = pageContent.querySelector('#gameMode');
    const mapSelect = pageContent.querySelector('#mapSelect');
    const mapPreview = pageContent.querySelector('#mapPreview');
    const startServerBtn = pageContent.querySelector('#startServerBtn');

    if (!gameModeSelect || !mapSelect) return;

    function populateGameModes(gamemodes) {
      gameModeSelect.innerHTML = '';

      gamemodes.forEach(gamemode => {
        const option = document.createElement('option');
        option.value = gamemode.id;
        option.textContent = gamemode.id;
        gameModeSelect.appendChild(option);
      });
    }

    function populateMaps(maps) {
      mapSelect.innerHTML = '';

      maps.forEach(map => {
        const option = document.createElement('option');
        option.value = map.id;
        option.textContent = map.id;
        map.preview = option.dataset.preview = 'images/maps/' + map.id + '.jpg';
        mapSelect.appendChild(option);
      });

      // Initialize with first map
      if (maps.length > 0) {
        updateMapPreview(maps[0]);
      }
    }

    GameUI.LoadServerVariables(function (availableMaps, availableGamemodes) {
      console.log('Loaded server variables:', availableMaps, availableGamemodes);
      populateGameModes(availableGamemodes);
      populateMaps(availableMaps);
    });

    // Event listener for map selection
    mapSelect.addEventListener('change', function () {
      const selectedOption = this.options[this.selectedIndex];
      const map = {
        id: selectedOption.value,
        name: selectedOption.textContent,
        preview: selectedOption.dataset.preview
      };

      updateMapPreview(map);
    });

    function updateMapPreview(map) {
      if (map.preview) {
        mapPreview.src = map.preview;
      } else {
        mapPreview.src = 'images/maps/_placeholder.jpg';
      }
    }

    // Start server button
    startServerBtn.addEventListener('click', function () {
      const serverName = pageContent.querySelector('#serverName').value || "My Awesome Server";
      const gameMode = gameModeSelect.value;
      const map = mapSelect.value;
      const maxPlayers = pageContent.querySelector('#maxPlayers').value;
      const password = pageContent.querySelector('#serverPassword').value;

      showLoadingOverlay('Starting Server...', function (finish) {
        // Create server configuration object
        const serverConfig = {
          name: serverName,
          gamemode: gameMode,
          map: map,
          maxPlayers: parseInt(maxPlayers, 10),
          password: password
        };

        window.GameUI.HostServer(serverConfig, function (wasSuccessful) {
          finish();
        });
      });
    });
  });

  window.GameUI.RequestServerList(function (wasSuccessful) {
    console.log('Server list requested:', wasSuccessful);
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

    LoadServerVariables: function (callback) {
      const mockMaps = [
        { id: 'gm_flatgrass' },
        { id: 'gm_construct' },
        { id: 'cs_office' },
        { id: 'de_dust2' },
        { id: 'c17_hospital' }
      ];

      const mockGamemodes = [
        { id: 'sandbox' },
        { id: 'deathmatch' },
        { id: 'teamdm' },
        { id: 'rp' },
        { id: 'zombies' }
      ];

      console.log(`Loading server variables (mock)`);
      setTimeout(() => {
        callback(mockMaps, mockGamemodes);
      }, 500);
    },

    HostServer: function (config, callback) {
      console.log(`Mocking host server with config:`, config);
      setTimeout(() => {
        callback(true);
      }, 2000);
    },

    RequestServerList: function (callback) {
      callback(true);
    },
  };
});
