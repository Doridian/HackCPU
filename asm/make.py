from sys import argv
from argparse import ArgumentParser
from transpiler import Transpiler

parser = ArgumentParser()
parser.add_argument("input", help="Input file", type=str)
parser.add_argument("output", help="Output file", type=str)
args = parser.parse_args()

transpiler = Transpiler(args.input, args.output)

transpiler.tryParse()
