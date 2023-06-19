"""\
This script converts an image to another one that is compatible with the
gameboy architecture. The resulting image can be safely used as input for
the png2asset binary from GBDK-2020 project.

Usage: image2gbpng.py input [--resize] -o OUTPUT 
"""
from PIL import Image, ImageOps

FINAL_PALETTE = [(0, 0, 0), (85, 85, 85), (170, 170, 170), (255, 255, 255)]


def get_closest_number_from_list(x: int, l: list):
    """given a number 'x', get the closest number from the list 'l' and its
    different from 'x'.

    Args:
        x (int): the number to find the clostest number from
        l (list): the list containing the numbers to check

    Returns:
        tuple(int, int): a tuple containing the closest number and its
        to the given number 'x'
    """
    closest = -1
    diff = -1
    for y in l:
        cur_diff = abs(y - x)
        if diff < 0 or cur_diff < diff:
            diff = cur_diff
            closest = y

    return closest, diff


def generate_gb_palette_mapping(palette: list) -> list:
    """generate a palette mapping from a given palette 'palette' to the
    gameboy palette: (0, 0, 0), (85, 85, 85), (170, 170, 170), (255, 255, 255).

    Example:
    if palette = [(86, 86, 86), (80, 80, 80), (85, 85, 85)],
    the function returns:
    {
        (80, 80, 80): (0, 0, 0),
        (85, 85, 85): (85, 85, 85),
        (86, 86, 86): (170, 170, 170)
    }

    Args:
        palette (list): a list with rgb colors.

    Returns:
        dict(list, list): a dictionary with input palette color as key and the
        mapped gameboy color as value
    """

    # only keep red component of color to simplify the folowing algorithm
    src_reds = [c[0] for c in palette]
    dest_reds = [c[0] for c in FINAL_PALETTE]

    src_reds.sort()
    dest_reds.sort()

    # dict that will store which src red is currently mapped with which
    # dest red.
    # finale example for palette = [(86, 86, 86), (80, 80, 80), (85, 85, 85)]
    # {
    #     0: {"diff": 80, "src_red": 80},
    #     85: {"diff": 0, "src_red": 85},
    #     170: {"diff": 84, "src_red": 86},
    #     255: {"diff": None, "src_red": None},
    # }
    chosen_dest_reds = {}
    for r in dest_reds:
        chosen_dest_reds[r] = {"src_red": None, "diff": None}

    # fill the chosen_dest_reds dict
    while src_reds:
        cur_red = src_reds.pop()
        cur_dest_reds = dest_reds.copy()

        while cur_dest_reds:
            closest, diff = get_closest_number_from_list(cur_red, cur_dest_reds)
            chosen_dest_red = chosen_dest_reds[closest]

            if chosen_dest_red["src_red"] == None:
                # if the chosen dest red is not already taken, the src red
                # will be mapped to it
                chosen_dest_red["src_red"] = cur_red
                chosen_dest_red["diff"] = diff
                cur_dest_reds = []
            else:
                if diff < chosen_dest_red["diff"]:
                    # if chosen dest red is already mapped but the difference
                    # with the mapped src red is bigger than the red we are
                    # processing, map the current red to the chosen dest red
                    # and put the src red to the src reds to process
                    src_reds.append(chosen_dest_red["src_red"])
                    chosen_dest_red["src_red"] = cur_red
                    chosen_dest_red["diff"] = diff
                    cur_dest_reds = []
                else:
                    # if chosen dest red is already mapped but the difference
                    # with the mapped src red is bigger than the red we are
                    # processing, we need to find another match. So we remove
                    # the dest red from the future potential results
                    cur_dest_reds.remove(closest)

    # create the palette map from the chosen_dest_reds
    # conversion example:
    # {
    #     0: {"diff": 80, "src_red": 80},
    #     85: {"diff": 0, "src_red": 85},
    #     170: {"diff": 84, "src_red": 86},
    #     255: {"diff": None, "src_red": None},
    # }
    # to
    # {
    #     (80, 80, 80): (0, 0, 0),
    #     (85, 85, 85): (85, 85, 85),
    #     (86, 86, 86): (170, 170, 170)
    # }
    palette_map = {}
    for k, v in chosen_dest_reds.items():
        src_red = v["src_red"]
        if src_red != None:
            src_color = (src_red, src_red, src_red)
            dest_color = (k, k, k)
            palette_map[src_color] = dest_color

    return palette_map


def reduce_image_for_gb(src_path: str, dest_path: str, resize: bool) -> None:
    """save a reduced version of the given image at 'src_path' to 'dest_path'

    Args:
        src_path (str): the file path to the image to reduce
        dest_path (str): the file path of the reduced image to be written
        resize (bool): True if image need to be resized to 160x144. False
        otherwise. The resize acts as a rescale and crop to avoid black format.
    """
    with Image.open(src_path) as image:
        if resize:
            # scale and crop to 160x144 within the image
            image.thumbnail((160, 144))
        # reduce to 4 colors
        image = image.convert("P", palette=Image.ADAPTIVE, colors=4)
        # turn image to grayscale
        image = ImageOps.grayscale(image)
        # convert to RGB (no alpha)
        image = image.convert("RGB")
        # get the current palette from the image and sort it
        current_palette = [t[1] for t in image.getcolors()]
        current_palette.sort()
        # create a mapping current palette -> new palette
        palettes_map = generate_gb_palette_mapping(current_palette)
        # loop over all pixels and switch to new palette color
        pixdata = image.load()
        for y in range(image.size[1]):
            for x in range(image.size[0]):
                pixdata[x, y] = palettes_map[pixdata[x, y]]
        # force the 8-bit
        image = image.quantize()
        # save the image to png
        image.save(dest_path, "png")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="Reduce an image for png2asset."
    )

    parser.add_argument("input", type=str, help="the path of the input image")
    parser.add_argument(
        "-o",
        "--output",
        help="the destination path of the reduced PNG",
        required=True,
    )
    parser.add_argument(
        "-r", "--resize", action="store_true", help="resize to 160x144"
    )

    args = parser.parse_args()

    if args.input and args.output:
        reduce_image_for_gb(args.input, args.output, args.resize)

        print("The image has been succesfully converted.")
