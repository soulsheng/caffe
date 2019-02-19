G:
cd ../../Build/x64
ssd_detect ../../models/VOC07/deploy.prototxt ../../models/VOC07/VGG_VOC07_SSD_300x300_iter_120000.caffemodel ../../data/files.txt --file_type=image --confidence_threshold=0.6
pause