import cv2
import numpy as np
import sys
import os

def preprocess_image(image_path:str, target_size=(28, 28))->np.ndarray:
    """reads and preprocesses the image by resizing
    and converting to grayscale."""
    image = cv2.imread(image_path)
    image = cv2.resize(image, target_size)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    
    output_directory = os.path.join(os.path.dirname(image_path), "resized_images")
    os.makedirs(output_directory, exist_ok=True)

    output_file = os.path.join(output_directory, os.path.basename(image_path))
    cv2.imwrite(output_file, image)


    return image

def save_to_header(data:np.ndarray, output_file:str):
    """Flattens the image array in row-major order
    and writes it as a C header file in int32 array format."""
    with open(output_file, 'w') as f:
        f.write('#ifndef IMAGE_DATA_H\n')
        f.write('#define IMAGE_DATA_H\n\n')
        f.write('#include <stdint.h>\n\n')
        f.write('const int image_data[] = {\n')

        for row in data:
            for value in row:
                f.write(f'{value}, ')
            f.write('\n')

        f.write('};\n\n')
        f.write('#endif // IMAGE_DATA_H\n')

if __name__ == "__main__":    
    if len(sys.argv) != 2:
        print("Usage: python script.py input_image output_header")
        sys.exit(1)


    image_path = sys.argv[1]
    output_file = f'{image_path.split('.')[0]}.h'

    processed_image = preprocess_image(image_path)
    save_to_header(processed_image, output_file)
