import png
import csv
#import matplotlib.pyplot as plt


for tx_num in range(0, 48):

    input_file = str(tx_num).zfill(5) + ".png"
    output_file = str(tx_num).zfill(5) + ".csv"

    reader = png.Reader(filename=input_file)
    w, h, pixels, metadata = reader.read_flat()
    pixel_byte_width = 4 if metadata['alpha'] else 3

    data = []
    for l in range(0, h):
        line = []
        for i in range(0, w):
            pos = i + l * w
            offset = pos * pixel_byte_width
            line.append(pixels[offset])
        data.append(line)
        #plt.plot(line)
        #plt.savefig(str(tx_num).zfill(2) + "_" + str(l).zfill(2) + ".png")

    with open(output_file, "w") as f:
        writer = csv.writer(f)
        writer.writerows(data)
