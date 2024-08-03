/** @type {import('tailwindcss').Config} */
module.exports = {
    content: [
        './**/*.{html,js}',
    ],
    theme: {
        extend: {
            colors: {
                'primary': {
                    '50': '#fdf4f3',
                    '100': '#fce7e4',
                    '200': '#fad3ce',
                    '300': '#f5b4ac',
                    '400': '#ed897c',
                    '500': '#e26151',
                    '600': '#ce4534',
                    '700': '#a33426',
                    '800': '#8f3125',
                    '900': '#772e25',
                    '950': '#40140f',
                },
                'back': '#201f1f',
            },
        },
    },
    plugins: [],
}

