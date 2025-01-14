#ifndef GAMEBOYCAMERA_H
#define GAMEBOYCAMERA_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QImage>
#include <QLabel>
#include <iostream>
#include <QSizePolicy>
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>

/*
 * Implementation data can be found here:
 * http://www.devrs.com/gb/files/gbcam.txt
 */

/**
 * @brief The GameboyCamera class
 *
 * Handles image extraction from gameboy camera save files
 */
class GameboyCamera : public QWidget
{
    Q_OBJECT

private:
    QByteArray camera_data;             // raw gameboy camera save data
    QPushButton *button_store_images;   // button to store images

    QVector<QImage> images_large;       // container holding large images
    QVector<QImage> image_small;        // container holding small images

    QVector<QByteArray> image_data_large;   // container holding raw image data (indices)
    QVector<QByteArray> image_data_small;   // container holding thumbnail data (indices)

    QGridLayout* picture_layout;            // layout holding pictures

    QListWidget *list_palettes;

    const QVector<uint8_t> color_templates = {
        0x9b,0xbc,0x0f,0x8b,0xac,0x0f,0x30,0x62,0x30,0x0f,0x38,0x0f,
        0xf8,0xe8,0xc8,0xd8,0x90,0x48,0xa8,0x28,0x20,0x30,0x18,0x50,
        0xd8,0xd8,0xc0,0xc8,0xb0,0x70,0xb0,0x50,0x10,0x00,0x00,0x00,
        0xf8,0xd8,0xb0,0x78,0xc0,0x78,0x68,0x88,0x40,0x58,0x38,0x20,
        0xff,0xff,0xff,0xa9,0xa9,0xa9,0x54,0x54,0x54,0x00,0x00,0x00
    };

public:
    /**
     * @brief default constructor
     * @param savegame data
     * @param parent object
     */
    explicit GameboyCamera(const QByteArray& _camera_data, QWidget *parent = nullptr);

private:
    /**
     * @brief extract image from raw data
     * @param image_id
     */
    void extract_image(unsigned int image_id);

    /**
     * @brief colorize a (large) image
     * @param image id
     * @param palette id
     * @return colorized image
     */
    QImage colorize_image(unsigned int image_id, unsigned int palette_id = 0);

    /**
     * @brief colorize a thumbnail image
     * @param image id
     * @param palette id
     * @return colorized image
     */
    QImage colorize_image_small(unsigned int image_id, unsigned int palette_id = 0);

    /**
     * @brief create color palette from raw color information
     * @param palette_id
     * @return image showing color palette
     */
    QImage create_color_palette(unsigned int palette_id);

private slots:
    /**
     * @brief update the color of the images by the selected color palette
     * @param palette_id
     */
    void update_color_images(int palette_id);

    /**
     * @brief Store all images to disk
     */
    void save_images();
};

#endif // GAMEBOYCAMERA_H
