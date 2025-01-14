#include "gameboycamera.h"

/**
 * @brief default constructor
 * @param savegame data
 * @param parent object
 */
GameboyCamera::GameboyCamera(const QByteArray& _camera_data, QWidget *parent) : QWidget(parent) {
    // set window properties
    this->setWindowIcon(QIcon(":/assets/img/logo.ico"));
    this->setWindowTitle("Gameboy Camera Images");

    this->camera_data = _camera_data;

    // build layout
    QHBoxLayout *layout = new QHBoxLayout();
    this->setLayout(layout);
    QWidget* picture_container = new QWidget(this);
    this->picture_layout = new QGridLayout();
    picture_container->setLayout(picture_layout);
    layout->addWidget(picture_container);

    // build user interaction interface
    QWidget* interface_container = new QWidget(this);
    QVBoxLayout* layout_interface = new QVBoxLayout();
    interface_container->setLayout(layout_interface);
    layout->addWidget(interface_container);

    // add color selector
    QLabel *color_scheme_label = new QLabel("Select color palette");
    layout_interface->addWidget(color_scheme_label);

    // add color palettes
    this->list_palettes = new QListWidget();
    for(int i=0; i<this->color_templates.size() / 12; i++) {
        QImage image_palette = this->create_color_palette(i);
        QListWidgetItem *itm = new QListWidgetItem();
        itm->setIcon(QIcon(QPixmap::fromImage(image_palette)));
        this->list_palettes->addItem(itm);
    }
    this->list_palettes->setIconSize(QSize(32*4,32));
    this->list_palettes->setMaximumWidth(5*32);
    layout_interface->addWidget(this->list_palettes);

    // add store image button
    this->button_store_images = new QPushButton("Save images");
    layout_interface->addWidget(this->button_store_images);

    // add spacer
    QFrame *frame = new QFrame();
    frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout_interface->addWidget(frame);

    this->image_data_large.resize(30);
    this->image_data_small.resize(30);

    // put color in Window
    for(unsigned int i=0; i<30; i++) {
        this->extract_image(i);
    }

    for(unsigned int i=0; i<5; i++) {
        for(unsigned int j=0; j<6; j++) {
            QLabel *label = new QLabel();
            picture_layout->addWidget(label, i, j);
            QPixmap pixmap = QPixmap::fromImage(this->colorize_image(i*6+j));
            label->setPixmap(pixmap);
        }
    }

    connect(this->list_palettes, SIGNAL(currentRowChanged(int)), this, SLOT(update_color_images(int)));
    connect(this->button_store_images, SIGNAL(released()), this, SLOT(save_images()));
}

/**
 * @brief extract image from raw data
 * @param image_id
 */
void GameboyCamera::extract_image(unsigned int image_id) {
    QByteArray image_large(128 * 112, 0x00);
    QByteArray image_small(32 * 32, 0x00);

    // extract image data
    unsigned int pos = (image_id+2) * 0x1000;                             // location of large image data
    const QByteArray lg_data = this->camera_data.mid(pos, 0xe00);         // extract bytes
    const QByteArray sm_data = this->camera_data.mid(pos + 0xe00, 256);   // extract bytes

    // capture large image
    for(unsigned int tile_y=0; tile_y<14; tile_y++) {
        for(unsigned int tile_x=0; tile_x<16; tile_x++) {
            unsigned int tpos = (tile_y * 16 + tile_x) * 16;
            const QByteArray tiledata = lg_data.mid(tpos, 16);
            for(uint8_t y=0; y<8; y++) {
                for(uint8_t x=0; x<8; x++) {
                    uint8_t tx = tile_x * 8 + x;
                    uint8_t ty = tile_y * 8 + y;

                    uint8_t upper = (tiledata[y * 2 + 1] >> (7-x)) & 0x01;
                    uint8_t lower = (tiledata[y * 2] >> (7-x)) & 0x01;

                    uint8_t cidx = (upper << 1) | lower;
                    image_large[ty * 128 + tx] = cidx;
                }
            }
        }
    }
    this->image_data_large[image_id] = image_large;

    // capture small image
    for(unsigned int tile_y=0; tile_y<4; tile_y++) {
        for(unsigned int tile_x=0; tile_x<4; tile_x++) {
            unsigned int tpos = (tile_y * 4 + tile_x) * 16;
            const QByteArray tiledata = sm_data.mid(tpos, 16);
            for(uint8_t y=0; y<8; y++) {
                for(uint8_t x=0; x<8; x++) {
                    uint8_t tx = tile_x * 8 + x;
                    uint8_t ty = tile_y * 8 + y;

                    uint8_t upper = (tiledata[y * 2 + 1] >> (7-x)) & 0x01;
                    uint8_t lower = (tiledata[y * 2] >> (7-x)) & 0x01;

                    uint8_t cidx = (upper << 1) | lower;
                    image_small[ty * 32 + tx] = cidx;
                }
            }
        }
    }
    this->image_data_small[image_id] = image_small;
}

/**
 * @brief colorize a (large) image
 * @param image id
 * @param palette id
 * @return colorized image
 */
QImage GameboyCamera::colorize_image(unsigned int image_id, unsigned int palette_id) {
    QImage image(128, 112, QImage::Format_RGB32);

    for(int y=0; y<112; y++) {
        for(int x=0; x<128; x++) {
            uint8_t color_id = this->image_data_large[image_id][y * 128 + x];
            QColor color(this->color_templates[12 * palette_id + color_id * 3],
                         this->color_templates[12 * palette_id + color_id * 3 + 1],
                         this->color_templates[12 * palette_id + color_id * 3 + 2]);
            image.setPixelColor(x, y, color);
        }
    }

    return image;
}

/**
 * @brief colorize a thumbnail image
 * @param image id
 * @param palette id
 * @return colorized image
 */
QImage GameboyCamera::colorize_image_small(unsigned int image_id, unsigned int palette_id) {
    QImage image(32, 32, QImage::Format_RGB32);

    for(int y=0; y<32; y++) {
        for(int x=0; x<32; x++) {
            uint8_t color_id = this->image_data_small[image_id][y * 32 + x];
            QColor color(this->color_templates[12 * palette_id + color_id * 3],
                         this->color_templates[12 * palette_id + color_id * 3 + 1],
                         this->color_templates[12 * palette_id + color_id * 3 + 2]);
            image.setPixelColor(x, y, color);
        }
    }

    return image;
}

/**
 * @brief create color palette from raw color information
 * @param palette_id
 * @return image showing color palette
 */
QImage GameboyCamera::create_color_palette(unsigned int palette_id) {
    QImage image(32*4, 32, QImage::Format_RGB32);

    for(unsigned int k=0; k<4; k++) {
        for(unsigned int y=0; y<32; y++) {
            for(unsigned int x=0; x<32; x++) {
                image.setPixelColor(k * 32 + x, y, QColor(this->color_templates[12 * palette_id + k * 3],
                                                          this->color_templates[12 * palette_id + k * 3 + 1],
                                                          this->color_templates[12 * palette_id + k * 3 + 2]));
            }
        }
    }

    return image;
}

/**
 * @brief update the color of the images by the selected color palette
 * @param palette_id
 */
void GameboyCamera::update_color_images(int palette_id) {
    for(unsigned int i=0; i<5; i++) {
        for(unsigned int j=0; j<6; j++) {
            QPixmap pixmap = QPixmap::fromImage(this->colorize_image(i*6+j, palette_id));
            reinterpret_cast<QLabel*>(picture_layout->itemAtPosition(i, j)->widget())->setPixmap(pixmap);
        }
    }
}

/**
 * @brief Store all images to disk
 */
void GameboyCamera::save_images() {
    QString folder = QFileDialog::getExistingDirectory(this, tr("Select save location"));
    if(folder.length() == 0) {
        return;
    }

    unsigned int count = 0;

    // store large images
    bool overwrite_ask = true;
    for(unsigned int i=0; i<30; i++) {
        QImage image = this->colorize_image(i, this->list_palettes->currentRow());
        QString filename = tr("%1_large.png").arg(i+1,2,10,QLatin1Char('0'));
        QString target = folder + QDir::separator() + filename;

        if(QFile::exists(target) && overwrite_ask == true) {
            QMessageBox msg_box;
            msg_box.setIcon(QMessageBox::Question);
            msg_box.setText("File " + filename + " already exists.");
            msg_box.setInformativeText("Are you sure you want to overwrite this file?");
            msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::Cancel);
            msg_box.setDefaultButton(QMessageBox::Yes);
            msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
            int ret = msg_box.exec();

            if(ret == QMessageBox::Cancel) {
                return;
            }

            if(ret == QMessageBox::YesToAll) {
                overwrite_ask = false;
            }

            if(ret == QMessageBox::Yes || ret == QMessageBox::YesToAll) {
                count++;
                image.save(target);
                continue;
            }
        }

        count++;
        image.save(target);
    }

    QMessageBox msg_box;
    msg_box.setIcon(QMessageBox::Question);
    msg_box.setText("Store thumbnail images");
    msg_box.setInformativeText("There are also 32x32 px thumbnails of the images available. Would you like to store these as well?");
    msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg_box.setDefaultButton(QMessageBox::Yes);
    msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
    int ret = msg_box.exec();

    if(ret == QMessageBox::Yes) {
        // store small images
        bool overwrite_ask = true;
        for(unsigned int i=0; i<30; i++) {
            QImage image = this->colorize_image_small(i, this->list_palettes->currentRow());
            QString filename = tr("%1_small.png").arg(i+1,2,10,QLatin1Char('0'));
            QString target = folder + QDir::separator() + filename;

            if(QFile::exists(target) && overwrite_ask == true) {
                QMessageBox msg_box;
                msg_box.setIcon(QMessageBox::Question);
                msg_box.setText("File " + filename + " already exists.");
                msg_box.setInformativeText("Are you sure you want to overwrite this file?");
                msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::Cancel);
                msg_box.setDefaultButton(QMessageBox::Yes);
                msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
                int ret = msg_box.exec();

                if(ret == QMessageBox::Cancel) {
                    return;
                }

                if(ret == QMessageBox::YesToAll) {
                    overwrite_ask = false;
                }

                if(ret == QMessageBox::Yes || ret == QMessageBox::YesToAll) {
                    count++;
                    image.save(target);
                    continue;
                }
            }

            count++;
            image.save(target);
        }
    }

    QMessageBox msg_box_done(QMessageBox::Information,
            "Done",
            tr("Succesfully saved %1 files to disk.").arg(count),
            QMessageBox::Ok, this);
    msg_box_done.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    msg_box_done.exec();
}
