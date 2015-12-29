// ----------------------------------------------------------------------------
// mitubeRenderView : a multiple-window itubeRender capsule, loading multiple
//					DTI voxel model each assigned with a separate window using
//					multiple process run-time modality
//
// Creation : Mar. 2nd 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "mitubeRenderView.h"

using std::cout;
using std::cerr;
using std::ostringstream;

///////////////////////////////////////////////////////////////////////////////
//
//	Implementation of the Class CMitubeRenderView
//
CMitubeRenderView* CMitubeRenderView::m_pmitInstance = NULL;
CMitubeRenderView::CMitubeRenderView(int argc, char **argv) : 
	CSitubeRender(argc, argv),
	m_bSync(true)
{
	addOption('f', true, "input-files", "the name of source file containing"
		   " geometry in the format of tgdata, each one given by a -f ");
	addOption('d', true, "dwi-b0-dirs", "directory holding DWI b0 DICOM images"
		   ",each one given by a -d ");
	addOption('i', true, "fiber-indices", "fiber indices");
}

CMitubeRenderView::~CMitubeRenderView()
{
	for (size_t i = 0; i < m_allargvs.size(); ++i) {
		delete [] m_allargvs[i];
		m_allargvs[i] = NULL;
	}

	for (size_t i = 0; i < m_renders.size(); ++i) {
		delete m_renders[i];
		m_renders[i] = NULL;
	}
}

int CMitubeRenderView::handleOptions(int optv) 
{
	switch( optv ) {
		case 'f':
			m_strfnsrcs.push_back(optarg);
			return 0;
		case 'd':
			m_strdwidirs.push_back( optarg );
			return 0;
		case 'i':
			m_strfiberidxs.push_back( optarg );
			return 0;
		case 'b':
		case 'l':
		case 'g':
		case 'r':
		case 'p':
		case 't':
			return 0;
		default:
			return CSitubeRender::handleOptions( optv );
	}
	return 1;
}

void CMitubeRenderView::keyResponse(unsigned char key, int x, int y) 
{
	switch (key) {
		case 27:
		case '0':
			return;
		default:
			break;
	}

	// when not in the synchronization mode, we only take care of the focused
	// window
	if ( !m_bSync ) {
		m_renders[ glutGetWindow() - 2 ]->keyResponse(key, x, y);
		glutPostRedisplay();
		return;
	}

	bool bAllfinished = true;
	for (int i = 0; i < (int)m_renders.size(); i++) {
		glutSetWindow(i+2);
		m_renders[i]->keyResponse(key, x, y);
		glutPostRedisplay();

		if ( !m_renders[i]->m_bTaskDone ) {
			bAllfinished = false;
		}
	}

	if ( key == 32 && bAllfinished ) {
		exit(EXIT_SUCCESS);
	}
}

void CMitubeRenderView::specialResponse(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_INSERT:
			return;
		case GLUT_KEY_END:
			return;
		case GLUT_KEY_F5:
			m_bSync = !m_bSync;
			return;
		default:
			break;
	}

	// when not in the synchronization mode, we only take care of the focused
	// window
	if ( !m_bSync ) {
		m_renders[ glutGetWindow() - 2 ]->specialResponse(key, x, y);
		glutPostRedisplay();
		return;
	}

	for (int i = 0; i < (int)m_renders.size(); i++) {
		glutSetWindow(i+2);
		m_renders[i]->specialResponse(key, x, y);
		glutPostRedisplay();
	}
}

void CMitubeRenderView::mouseResponse(int button, int state, int x, int y)
{
	// when not in the synchronization mode, we only take care of the focused
	// window
	int curRenderIdx = glutGetWindow() - 2;
	if ( !m_bSync ) {
		m_renders[ curRenderIdx ]->mouseResponse(button, state, x, y);
		glutPostRedisplay();
		return;
	}

	GLdouble rx = 1.0, ry = 1.0;
	for (int i = 0; i < (int)m_renders.size(); i++) {
		glutSetWindow(i+2);
		rx = (m_renders[i]->m_maxCoord[0] - m_renders[i]->m_minCoord[0])*1.0/ 
			(m_renders[curRenderIdx]->m_maxCoord[0] - m_renders[curRenderIdx]->m_minCoord[0]);
		ry = (m_renders[i]->m_maxCoord[1] - m_renders[i]->m_minCoord[1])*1.0/ 
			(m_renders[curRenderIdx]->m_maxCoord[1] - m_renders[curRenderIdx]->m_minCoord[1]);

		m_renders[i]->mouseResponse(button, state, x, y);
		glutPostRedisplay();
	}
}

void CMitubeRenderView::mouseMotionResponse(int x, int y)
{
	// when not in the synchronization mode, we only take care of the focused
	// window
	int curRenderIdx = glutGetWindow() - 2;
	if ( !m_bSync ) {
		m_renders[ curRenderIdx ]->mouseMotionResponse(x, y);
		glutPostRedisplay();
		return;
	}

	GLdouble rx = 1.0, ry = 1.0;
	for (int i = 0; i < (int)m_renders.size(); i++) {
		glutSetWindow(i+2);
		rx = (m_renders[i]->m_maxCoord[0] - m_renders[i]->m_minCoord[0])*1.0/ 
			(m_renders[curRenderIdx]->m_maxCoord[0] - m_renders[curRenderIdx]->m_minCoord[0]);
		ry = (m_renders[i]->m_maxCoord[1] - m_renders[i]->m_minCoord[1])*1.0/ 
			(m_renders[curRenderIdx]->m_maxCoord[1] - m_renders[curRenderIdx]->m_minCoord[1]);
		m_renders[i]->mouseMotionResponse(x, y);
		glutPostRedisplay();
	}
}

void CMitubeRenderView::mousePassiveMotionResponse(int x, int y)
{
	// when not in the synchronization mode, we only take care of the focused
	// window
	int curRenderIdx = glutGetWindow() - 2;
	if ( !m_bSync ) {
		m_renders[ curRenderIdx ]->mousePassiveMotionResponse(x, y);
		glutPostRedisplay();
		return;
	}

	//GLdouble rx = 1.0, ry = 1.0;
	for (int i = 0; i < (int)m_renders.size(); i++) {
		glutSetWindow(i+2);
		/*
		rx = (m_renders[i]->m_maxCoord[0] + m_renders[i]->m_minCoord[0])*1.0/ 
			(m_renders[curRenderIdx]->m_maxCoord[0] + m_renders[curRenderIdx]->m_minCoord[0]);
		ry = (m_renders[i]->m_maxCoord[1] + m_renders[i]->m_minCoord[1])*1.0/ 
			(m_renders[curRenderIdx]->m_maxCoord[1] + m_renders[curRenderIdx]->m_minCoord[1]);
		rx = ( m_renders[i]->m_boxes[0].m_winboxvertices[7].x - m_renders[i]->m_boxes[0].m_winboxvertices[2].x )*1.0 /
		     ( m_renders[curRenderIdx]->m_boxes[0].m_winboxvertices[7].x - m_renders[curRenderIdx]->m_boxes[0].m_winboxvertices[2].x );
		ry = ( m_renders[i]->m_boxes[0].m_winboxvertices[7].y - m_renders[i]->m_boxes[0].m_winboxvertices[2].y )*1.0 /
		     ( m_renders[curRenderIdx]->m_boxes[0].m_winboxvertices[7].y - m_renders[curRenderIdx]->m_boxes[0].m_winboxvertices[2].y );
		*/

		m_renders[i]->mousePassiveMotionResponse(x, y);
		glutPostRedisplay();
	}
}

void CMitubeRenderView::mouseWheelRollResponse(int wheel, int direction, int x, int y)
{
	// when not in the synchronization mode, we only take care of the focused
	// window
	if ( !m_bSync ) {
		m_renders[ glutGetWindow() - 2 ]->mouseWheelRollResponse(
				wheel, direction, x, y);
		glutPostRedisplay();
		return;
	}

	for (int i = 0; i < (int)m_renders.size(); i++) {
		glutSetWindow(i+2);
		m_renders[i]->mouseWheelRollResponse(wheel, direction, x, y);
		glutPostRedisplay();
	}
}

void CMitubeRenderView::onReshape( GLsizei w, GLsizei h )
{
	/*
	cout << "in Reshape:: this windows is : " << glutGetWindow() << "\n";
	*/
	int idx = glutGetWindow();
	if ( idx - 2 >= 0 && idx - 2 < (int)(m_pmitInstance->m_renders.size()) ) {
		m_pmitInstance->m_renders[idx-2]->onReshape(w, h);
	}
}

void CMitubeRenderView::display( void )
{
	/*
	cout << "in display:: this windows is : " << glutGetWindow() << "\n";
	*/
	int idx = glutGetWindow();
	if ( idx -2 >= 0 && idx - 2 < (int)(m_pmitInstance->m_renders.size()) ) {
		// dispatching drawing process to window-associated render
		m_pmitInstance->m_renders[idx-2]->display();
	}
}

int CMitubeRenderView::mainstay() 
{
	if ( 0 != _createAllRenders() ) {
		cerr << "FATAL : failed to create sitRenders.\n";
		return -1;
	}
	
	if ( 0 != _armRenders() ) {
		return -1;
	}

	m_pInstance = this;
	m_pmitInstance = this;

	glutInit (&m_argc, m_argv);
	glutInitDisplayMode	( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glInit();

	_createAllWindows();

	glutMainLoop();
	return 0;
}

/*
 * we set a layout in which each row holds at most two subwindows, then
 * followed by other rows if needed
 */
int CMitubeRenderView::_createAllWindows()
{
	int nRender = (int)m_renders.size();
	if ( nRender < 1 ) {
		return -1;
	}

	int w, h, x = 0, y = 0;
	CGLApp::getScreenSize(&w, &h);

	h -= 80;

	int mw = 0;
	glutInitWindowPosition ( x, y );
	glutInitWindowSize ( w, h );
	mw = glutCreateWindow ( "multiple-view DTI model render" );
	glutPositionWindow(x, y);

	w /= (nRender<=1?1:2);
	h /= nRender<=1?1:(int)(1.0*nRender/2+.5); 

	for (int i = 0; i < nRender; i++) {
		if ( i % 2 == 0 ) {
			x = 0;
		}
		else {
			x = w;
		}

		if ( i != 0 && i % 2 == 0 ) {
			y += h;
		}

		ostringstream ostrTitle;
		ostrTitle << " DTI voxel model render - " << i;
		m_renders[i]->setGlutInfo(ostrTitle.str().c_str(),
				x, y, w, h);

		glutCreateSubWindow(mw, x, y, w, h);

		/* all windows will all share the same interaction input */
		glutMouseFunc(_mouseResponse);
		glutMotionFunc(_mouseMotionResponse);
		glutPassiveMotionFunc(_mousePassiveMotionResponse);
		//glutMouseWheelFunc(_mouseWheelRollResponse);
		glutKeyboardFunc(_keyResponse);
		glutSpecialFunc(_specialResponse);

		/* but not the drawing stuff */
		glutReshapeFunc(_onReshape);
		glutDisplayFunc(_display);

		/* this is the MOST ESSENTIAL! Ignorance of this had ever cheated me out
		 * of my valued time for more than 3 hours*/
		m_renders[i]->glInit();
	}

	return 0;
}

int CMitubeRenderView::_assign_argvs(int renderIdx)
{
	// number of common arguments
	int nItmes = m_argc - (int)m_strfnsrcs.size()*2 - (int)m_strdwidirs.size()*2 - (int)m_strfiberidxs.size()*2;
	// -f is compulsory
	nItmes +=2;
	// -d is optional, the dwi dirs will be assigned from left to right when
	// it is exhausted, the later render will have not got this argument 
	if ( renderIdx < (int)m_strdwidirs.size() ) {
		nItmes+=2;
	}

	// -i if also compulsory
	nItmes += 2;

	m_allargvs[renderIdx] = new char*[nItmes];
	int cntf = 0, cntd = 0, cntthis = 0, cnti = 0;
	// scan the gross argv in m_argv and pick up to assign to each render
	for (int i = 0; i < m_argc; i++) {
		if ( 0 == strncmp( m_argv[i], "-f", 2 ) ) {
			if (renderIdx == cntf) {
				m_allargvs[renderIdx][cntthis++] = m_argv[i];
				m_allargvs[renderIdx][cntthis++] = m_argv[i+1];
			}
			i ++;
			cntf ++;
			continue;
		}

		if ( 0 == strncmp( m_argv[i], "-d", 2 ) ) {
			if (renderIdx == cntd) {
				m_allargvs[renderIdx][cntthis++] = m_argv[i];
				m_allargvs[renderIdx][cntthis++] = m_argv[i+1];
			}
			i ++;
			cntd ++;
			continue;
		}

		if ( 0 == strncmp( m_argv[i], "-i", 2 ) ) {
			if (renderIdx == cnti) {
				m_allargvs[renderIdx][cntthis++] = m_argv[i];
				m_allargvs[renderIdx][cntthis++] = m_argv[i+1];
			}
			i ++;
			cnti ++;
			continue;
		}

		// others are all common arguments, just collect as ther are
		m_allargvs[renderIdx][cntthis++] = m_argv[ i ];
	}
	return nItmes;
}

int CMitubeRenderView::_createAllRenders()
{
	int nRender = static_cast<int>( m_strfnsrcs.size() );
	if ( nRender < 1 ) {
		// nothing to do, let it go
		return -1;
	}

	m_allargvs.resize ( nRender );
	m_allargvs.assign( nRender, (char**)NULL );

	/* prepare command line arguments for each render
	 * since "-d" is optional and "-f" is compulsory, we count the number of
	 * input arguments for "-f" and presume the same number of renders
	 * will be needed
	 */
	int nItmes;
	for (int i = 0; i < nRender; i++) {
		nItmes = _assign_argvs( i );
		if ( nItmes < 1 ) {
			return -1;
		}

		m_cout << "----- command line for process " << i << " -------\n";
		for (int j = 0; j < nItmes; j++) {
			m_cout << m_allargvs[i][j] << " ";
		}
		m_cout << "\n";

		m_renders.push_back( new CSitubeRender( nItmes, m_allargvs[ i ] ) );
	}

	return 0;
}

int CMitubeRenderView::_armRenders() 
{
	int nRender = (int)m_renders.size();
	if ( nRender < 1 ) {
		return -1;
	}

	for (int i = 0; i < nRender; i++) {
		m_renders[i]->run();

		unsigned long szTotal = m_renders[i]->m_loader.getSize();
		for (unsigned long idx = 0; idx < szTotal; ++idx) {
			vector<GLfloat> & line = m_renders[i]->m_loader.getElement( idx );
			unsigned long szPts = static_cast<unsigned long>( line.size()/6 );

			for (unsigned long idx1 = 0; idx1 < szPts; idx1++) {
				line [ idx1*6 + 3 ] *= 
					(m_renders[0]->m_loader.m_maxCoord[0])/
					(m_renders[i]->m_loader.m_maxCoord[0]);
					/*
					(m_renders[0]->m_loader.m_maxCoord[0]+m_renders[0]->m_loader.m_minCoord[0])/
					(m_renders[i]->m_loader.m_maxCoord[0]+m_renders[0]->m_loader.m_minCoord[0]);
					*/
				line [ idx1*6 + 4 ] *= 
					(m_renders[0]->m_loader.m_maxCoord[1])/
					(m_renders[i]->m_loader.m_maxCoord[1]);
					/*
					(m_renders[0]->m_loader.m_maxCoord[1]+m_renders[0]->m_loader.m_minCoord[1])/
					(m_renders[i]->m_loader.m_maxCoord[1]+m_renders[0]->m_loader.m_minCoord[1]);
					*/
				line [ idx1*6 + 5 ] *= 
					(m_renders[0]->m_loader.m_maxCoord[2])/
					(m_renders[i]->m_loader.m_maxCoord[2]);
					/*
					(m_renders[0]->m_loader.m_maxCoord[2]+m_renders[0]->m_loader.m_minCoord[2])/
					(m_renders[i]->m_loader.m_maxCoord[2]+m_renders[0]->m_loader.m_minCoord[2]);
					*/
			}
		}

		for (int j = 0;  j < 3; j++) {
			m_renders[i]->m_loader.m_minCoord[j] = m_renders[0]->m_loader.m_minCoord[j];
			m_renders[i]->m_loader.m_maxCoord[j] = m_renders[0]->m_loader.m_maxCoord[j];
		}
	}

	for (int i = 0; i < nRender; i++) {
		m_renders[i]->run();

		for (int j = 0;  j < 3; j++) {
			m_renders[i]->m_minCoord[j] = m_renders[0]->m_minCoord[j];
			m_renders[i]->m_maxCoord[j] = m_renders[0]->m_maxCoord[j];
		}

		/*
		m_renders[i]->setPrjInfo(45.0f, 
				ABS(m_renders[0]->m_minCoord[2])/4.0,
				(ABS(m_renders[0]->m_maxCoord[2]) + ABS(m_renders[0]->m_minCoord[2]))*8);

		m_renders[i]->setViewInfo(0.0, 0.0,
				( m_renders[0]->m_minCoord[2] + m_renders[0]->m_maxCoord[2] )*3,
				0.0, 0.0, 0.0,
				0, 1, 0);
		*/
	}

	for (int i = 0; i < nRender; i++) {
		m_renders[i]->suspend(true);
		// user never need repeated block of help prompt
		m_renders[i]->switchhelp(0 == i);
		m_renders[i]->run();

		if ( i == 0 ) {
			continue;
		}
		/*
		m_renders[i]->setViewInfo(0.0, 0.0,
			( m_renders[i]->m_minCoord[2] + m_renders[i]->m_maxCoord[2] )*3 /
			( ( m_renders[i]->m_minCoord[2] + m_renders[i]->m_maxCoord[2] )*1.0/
			 ( m_renders[0]->m_minCoord[2] + m_renders[0]->m_maxCoord[2] ) ),
			0.0, 0.0, 0.0,
			0, 1, 0);
		*/
	}

	return 0;
}

/* set ts=4 sts=4 tw=80 sw=4 */

