(defun RALpng2cds ()
  png2gds = "/path/to/bin/png2gds"

  cv = deGetEditCellView()

  (if cv then
	pngfile = (buildString (hiGetExistingFile ?filter "PNG Images (*.png)" ?caption "Import PNG" ?workingDir (getShellEnvVar "HOME")) "/")

	(if pngfile != "" then
	  tech = (techGetTechFile cv~>lib)
	  grid = (techGetMfgGridResolution tech)
	  validLayers = (listToVector (leGetValidLayerList tech))
	  layerCount = (length validLayers)

	  cid = (ipcBeginProcess (strcat png2gds " " pngfile " stdout 1"))

	  x1 = 0
	  y1 = 0
	  x2 = 0
	  y2 = 0
	  partialString = ""

	  (if cid != nil then
		(ipcWaitForProcess cid)

		(while data = (ipcReadProcess cid 10)
			   /* A line is formated as: layer x1 y1 x2 y2 */
			   data = (strcat partialString data)
			   partialString = ""

			   lines = (parseString data "\n")
			   (foreach line lines
						args = (sscanf line "%d %d %d %d %d " layer x1 y1 x2 y2)
						x1 = x1 * grid
						y1 = y1 * grid
						x2 = x2 * grid
						y2 = y2 * grid
						(if args == 5 then
						  bbox = (list (list x1 y1) (list x2 y2))
						  (if (isBBox bbox) == t then
							(if (dbCreateRect cv validLayers[(mod layer layerCount)] bbox) == nil then
							  (printf "Bad dbCreateRect: %s\n" line))
							else
							(printf "Bad bbox: %s\n" line))
						  else
						  partialString = line
						  )
						)
			   )
		(if (ipcIsAliveProcess cid) then
		  (ipcKillProcess cid))
		)
	  )
	)
  )

