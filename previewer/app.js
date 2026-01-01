const gradientMap = [[{r: 255, g: 100, b: 190}, {r: 200, g: 20, b: 141}], [{r: 51, g: 20, b: 190}, {
    r: 255, g: 105, b: 200
}]];


function preprocessGradient(gradientMap) {
    const height = 32;
    const width = 64;
    const gradient = [];

    for (let y = 0; y < height; y++) {
        gradient[y] = [];
        for (let x = 0; x < width; x++) {
            const fy = y / (height - 1);
            const fx = x / (width - 1);

            const topColor = interpolateColor(gradientMap[0][0], gradientMap[0][1], fx);
            const bottomColor = interpolateColor(gradientMap[1][0], gradientMap[1][1], fx);

            gradient[y][x] = interpolateColor(topColor, bottomColor, fy);
        }
    }

    return gradient;
}

function interpolateColor(c1, c2, t) {
    return {
        r: Math.round(c1.r + (c2.r - c1.r) * t),
        g: Math.round(c1.g + (c2.g - c1.g) * t),
        b: Math.round(c1.b + (c2.b - c1.b) * t)
    };
}


class Texture {
    constructor(width = 64, height = 32) {
        this.width = width;
        this.height = height;
        this.textureMap = Array(height).fill(null).map(() => Array(width).fill(0));
    }

    loadFromImage(image, channel) {
        const canvas = document.createElement('canvas');
        canvas.width = this.width;
        canvas.height = this.height;
        const ctx = canvas.getContext('2d');

        ctx.drawImage(image, 0, 0, this.width, this.height);
        const imageData = ctx.getImageData(0, 0, this.width, this.height);
        const data = imageData.data;

        for (let y = 0; y < this.height; y++) {
            for (let x = 0; x < this.width; x++) {
                const idx = (y * this.width + x) * 4;
                const r = data[idx];
                const g = data[idx + 1];
                const b = data[idx + 2];
                const a = data[idx + 3];

                let value = 0;

                switch (channel) {
                    case 'ALPHA':
                        value = a / 255.0;
                        break;
                    case 'RED':
                        if (r === 255) value = a / 255.0;
                        break;
                    case 'GREEN':
                        if (g === 255) value = a / 255.0;
                        break;
                    case 'BLUE':
                        if (b === 255) value = a / 255.0;
                        break;
                }

                this.textureMap[y][x] = value;
            }
        }
    }
}


class PreviewApp {
    constructor() {
        this.eyeTexture = null;
        this.eyePupilArea = null;
        this.eyePupilShape = null;
        this.mouthTexture = null;

        this.gradient = preprocessGradient(gradientMap);
        this.animationTime = 0;
        this.animationFrame = null;

        this.animateEnabled = true;
        this.manualPupilX = 0.5;
        this.manualPupilY = 0.5;

        this.setupCanvas();
        this.setupDropzones();
        this.setupControls();
        this.startAnimation();
    }

    setupCanvas() {
        this.canvas = document.getElementById('matrix-canvas');
        this.canvasGlow = document.getElementById('matrix-canvas-glow');
        this.ctx = this.canvas.getContext('2d');
        this.ctxGlow = this.canvasGlow.getContext('2d');
        this.ctx.imageSmoothingEnabled = false;
        this.ctxGlow.imageSmoothingEnabled = false;
    }

    setupDropzones() {
        this.setupDropzone('eye', (image) => {
            this.eyeTexture = new Texture();
            this.eyePupilArea = new Texture();
            this.eyePupilShape = new Texture();

            this.eyeTexture.loadFromImage(image, 'BLUE');
            this.eyePupilArea.loadFromImage(image, 'RED');
            this.eyePupilShape.loadFromImage(image, 'GREEN');
        });

        this.setupDropzone('mouth', (image) => {
            this.mouthTexture = new Texture();
            this.mouthTexture.loadFromImage(image, 'ALPHA');
        });
    }

    setupDropzone(type, onImageLoad) {
        const dropzone = document.getElementById(`${type}-dropzone`);
        const input = document.getElementById(`${type}-upload`);
        const preview = document.getElementById(`${type}-preview`);

        dropzone.addEventListener('click', () => input.click());

        dropzone.addEventListener('dragover', (e) => {
            e.preventDefault();
            dropzone.classList.add('dragover');
        });

        dropzone.addEventListener('dragleave', () => {
            dropzone.classList.remove('dragover');
        });

        dropzone.addEventListener('drop', (e) => {
            e.preventDefault();
            dropzone.classList.remove('dragover');

            const file = e.dataTransfer.files[0];
            if (file && file.type === 'image/png') {
                this.loadImageFile(file, dropzone, preview, onImageLoad);
            }
        });

        input.addEventListener('change', (e) => {
            const file = e.target.files[0];
            if (file) {
                this.loadImageFile(file, dropzone, preview, onImageLoad);
            }
        });
    }

    loadImageFile(file, dropzone, preview, onImageLoad) {
        const reader = new FileReader();
        reader.onload = (e) => {
            const img = new Image();
            img.onload = () => {
                dropzone.classList.add('has-image');

                const previewCtx = preview.getContext('2d');
                previewCtx.imageSmoothingEnabled = false;
                previewCtx.drawImage(img, 0, 0, 64, 32);

                onImageLoad(img);
            };
            img.src = e.target.result;
        };
        reader.readAsDataURL(file);
    }

    setupControls() {
        const animateToggle = document.getElementById('animate-toggle');
        const manualControls = document.getElementById('manual-controls');
        const pupilXSlider = document.getElementById('pupil-x-slider');
        const pupilYSlider = document.getElementById('pupil-y-slider');
        const pupilXValue = document.getElementById('pupil-x-value');
        const pupilYValue = document.getElementById('pupil-y-value');

        animateToggle.addEventListener('change', (e) => {
            this.animateEnabled = e.target.checked;
            manualControls.style.display = e.target.checked ? 'none' : 'flex';
        });

        pupilXSlider.addEventListener('input', (e) => {
            this.manualPupilX = parseFloat(e.target.value);
            pupilXValue.textContent = this.manualPupilX.toFixed(2);
        });

        pupilYSlider.addEventListener('input', (e) => {
            this.manualPupilY = parseFloat(e.target.value);
            pupilYValue.textContent = this.manualPupilY.toFixed(2);
        });

        document.getElementById('reset-btn').addEventListener('click', () => {
            this.reset();
        });
    }

    reset() {
        this.eyeTexture = null;
        this.eyePupilArea = null;
        this.eyePupilShape = null;
        this.mouthTexture = null;

        ['eye', 'mouth'].forEach(type => {
            const dropzone = document.getElementById(`${type}-dropzone`);
            const input = document.getElementById(`${type}-upload`);
            const preview = document.getElementById(`${type}-preview`);

            dropzone.classList.remove('has-image');
            input.value = '';

            const ctx = preview.getContext('2d');
            ctx.clearRect(0, 0, preview.width, preview.height);
        });
    }

    startAnimation() {
        const animate = () => {
            this.animationTime += 0.016 * 50;
            this.render();
            this.animationFrame = requestAnimationFrame(animate);
        };
        animate();
    }

    render() {
        const width = 128;
        const height = 32;

        this.ctx.fillStyle = '#000';
        this.ctx.fillRect(0, 0, width, height);

        this.ctxGlow.fillStyle = '#000';
        this.ctxGlow.fillRect(0, 0, width, height);

        if (this.eyeTexture && this.eyePupilArea && this.eyePupilShape) {
            this.renderPupils();
        }

        if (this.eyeTexture) {
            this.renderImage(this.eyeTexture, 0, false);
            this.renderImage(this.eyeTexture, 64, true);
        }

        if (this.mouthTexture) {
            this.renderImage(this.mouthTexture, 0, false);
            this.renderImage(this.mouthTexture, 64, true);
        }
    }

    renderPupils() {
        let pupilOffsetX_left, pupilOffsetX_right, pupilOffsetY;

        if (this.animateEnabled) {
            const time = this.animationTime;
            const oscillationX = Math.sin(time * 0.04 * 0.5) * 0.5;
            const oscBase = oscillationX * 1.1;

            pupilOffsetX_left = Math.max(0, Math.min(1, oscBase + 1.0)) - 0.2;
            pupilOffsetX_right = Math.max(0, Math.min(1, 1.0 - oscBase)) - 0.2;
            pupilOffsetY = 0.5;
        } else {
            pupilOffsetX_left = this.manualPupilX;
            pupilOffsetX_right = this.manualPupilX;
            pupilOffsetY = this.manualPupilY;
        }

        this.drawPupil(pupilOffsetX_left, pupilOffsetY, false);
        this.drawPupil(pupilOffsetX_right, pupilOffsetY, true);
    }

    drawPupil(eyeX, eyeY, mirror) {
        let eyeMinX = Infinity, eyeMaxX = -Infinity;
        let eyeMinY = Infinity, eyeMaxY = -Infinity;

        for (let y = 0; y < 32; y++) {
            for (let x = 0; x < 64; x++) {
                if (this.eyePupilArea.textureMap[y][x] > 0) {
                    eyeMinX = Math.min(eyeMinX, x);
                    eyeMaxX = Math.max(eyeMaxX, x);
                    eyeMinY = Math.min(eyeMinY, y);
                    eyeMaxY = Math.max(eyeMaxY, y);
                }
            }
        }

        if (!isFinite(eyeMinX)) return;

        let shapeMinX = Infinity, shapeMaxX = -Infinity;
        let shapeMinY = Infinity, shapeMaxY = -Infinity;

        for (let y = 0; y < 32; y++) {
            for (let x = 0; x < 64; x++) {
                if (this.eyePupilShape.textureMap[y][x] > 0) {
                    shapeMinX = Math.min(shapeMinX, x);
                    shapeMaxX = Math.max(shapeMaxX, x);
                    shapeMinY = Math.min(shapeMinY, y);
                    shapeMaxY = Math.max(shapeMaxY, y);
                }
            }
        }

        if (!isFinite(shapeMinX)) return;

        const baselineX = Math.floor((shapeMinX + shapeMaxX) / 2);
        const baselineY = Math.floor((shapeMinY + shapeMaxY) / 2);

        const redWidth = eyeMaxX - eyeMinX;
        const redHeight = eyeMaxY - eyeMinY;

        const yOffset = 0.5 - eyeY;
        const yMovement = Math.round(yOffset * redHeight);

        const xOffset = eyeX - 0.5;
        const xMovement = Math.round(xOffset * redWidth);

        const pupilCenterX = baselineX + xMovement;
        const pupilCenterY = baselineY + yMovement;

        const shapeCenterX = Math.floor((shapeMinX + shapeMaxX) / 2);
        const shapeCenterY = Math.floor((shapeMinY + shapeMaxY) / 2);

        const offsetX = pupilCenterX - shapeCenterX;
        const offsetY = pupilCenterY - shapeCenterY;

        for (let y = 0; y < 32; y++) {
            for (let x = 0; x < 64; x++) {
                if (this.eyePupilShape.textureMap[y][x] > 0) {
                    const px = x + offsetX;
                    const py = y + offsetY;

                    if (py >= 0 && py < 32 && px >= 0 && px < 64) {
                        const alpha = this.eyePupilArea.textureMap[py][px];
                        if (alpha > 0) {
                            this.setPixel(px, py, alpha, mirror);
                        }
                    }
                }
            }
        }
    }

    renderImage(texture, xOffset, mirror) {
        for (let y = 0; y < 32; y++) {
            for (let x = 0; x < 64; x++) {
                const alpha = texture.textureMap[y][x];
                if (alpha > 0) {
                    this.setPixel(x, y, alpha, mirror);
                }
            }
        }
    }

    setPixel(x, y, alpha, mirror) {
        if (alpha <= 0) return;

        const displayY = y;

        const col = this.gradient[displayY][x];

        const r = Math.round(col.r * alpha);
        const g = Math.round(col.g * alpha);
        const b = Math.round(col.b * alpha);

        this.ctx.fillStyle = `rgb(${r},${g},${b})`;
        this.ctxGlow.fillStyle = `rgb(${r},${g},${b})`;

        if (!mirror) {
            this.ctx.fillRect(x, displayY, 1, 1);
            this.ctxGlow.fillRect(x, displayY, 1, 1);
        } else {
            this.ctx.fillRect(128 - x - 1, displayY, 1, 1);
            this.ctxGlow.fillRect(128 - x - 1, displayY, 1, 1);
        }
    }
}

document.addEventListener('DOMContentLoaded', () => {
    new PreviewApp();
});
