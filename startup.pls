(begin

	; Creation of a point object
	; Returns a list with property "object-name": "point", "size": 0
	(define make-point (lambda (x y)
		(set-property "object-name" "point"
		(set-property "size" 0
			(list x y)
		))
	))

	; Creation of a line object

	; Creation of a text object
)
