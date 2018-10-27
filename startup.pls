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
	; Returns a list of two points with property "object-name": "line", "thickness": 1
	(define make-line (lambda (p1 p2)
		(set-property "object-name" "line"
		(set-property "thickness" 1
			(list p1 p2)
		))
	))

	; Creation of a text object
	; Returns a string literal with property "object-name": "text", "position": (0 0)
	(define make-text (lambda (str)
		(set-property "object-name" "text"
		(set-property "position" (make-point 0 0)
			(str)
		))
	))
)
