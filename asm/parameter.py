from defs import REG_CREG, REG_MREG, REG_MREGC, REGISTERS, BYTEORDER

class Parameter:
	def __init__(self, src):
		self.raw = src
		try:
			self.__parse(str(src).strip())
			self.parse_error = None
		except Exception as e:
			self.parse_error = e

	def __parse(self, src, mem=False):
		if src[0] == "[":
			if mem:
				raise ValueError("Nested [ are not allowed")
			if src[-1] != "]":
				raise ValueError("Missing ] after [")
			return self.__parse(src[1:-1], True)

		__creg = REG_CREG
		if mem:
			__creg = REG_MREGC

		srcsplit = src.split(" ")
		offset = 0
		if len(srcsplit) > 1:
			if len(srcsplit) % 2 != 1:
				raise ValueError("Invalid offset specifier length")
			for i in range(1, len(srcsplit), 2):
				if srcsplit[i] == "+":
					offset += int(srcsplit[i + 1], 0)
				elif srcsplit[i] == "-":
					offset -= int(srcsplit[i + 1], 0)
				else:
					raise ValueError("Invalid operator %s" % srcsplit[i])
			src = srcsplit[0]

		if src[0] == ":":
			self.rval = __creg
			self.cval = src[1:]
			self.cval_offset = offset
		elif src in REGISTERS:
			if mem:
				cval = REGISTERS[src]
				self.rval = REG_MREG
				if offset != 0:
					if offset < -128 or offset > 128:
						raise ValueError("This parameter only supports offsets within the inclusive range -128 to 128")
					if offset > 0:
						offset -= 1
					cval |= 0b10000
					self.cval = cval.to_bytes(1, BYTEORDER) + offset.to_bytes(1, BYTEORDER, signed=True)
				else:
					self.cval = cval.to_bytes(1, BYTEORDER)
			else:
				if offset != 0:
					raise ValueError("This parameter does not support offsets")
				self.rval = REGISTERS[src]
				self.cval = None
		else:
			try:
				self.rval = __creg
				self.cval = int(src, 0) + offset
			except:
				self.rval = __creg
				self.cval = src
				self.cval_offset = offset

	def raise_parse_error(self):
		if self.parse_error != None:
			raise self.parse_error

	def len(self, b64):
		self.raise_parse_error()

		if self.cval == None:
			return 0
		if isinstance(self.cval, bytes):
			return len(self.cval)
		if self.rval == REG_CREG and b64:
			return 8
		return 4

	def getcval(self, transpiler, b64):
		self.raise_parse_error()

		if isinstance(self.cval, str):
			lbl = transpiler.labels[self.cval]
			if isinstance(lbl, int):
				self.cval = lbl + self.cval_offset
			else:
				self.cval = transpiler.labels[self.cval].bpos + transpiler.baseaddr + self.cval_offset

		if isinstance(self.cval, int):
			if self.rval == REG_CREG and b64:
				return self.cval.to_bytes(8, BYTEORDER)
			return self.cval.to_bytes(4, BYTEORDER)

		return self.cval

