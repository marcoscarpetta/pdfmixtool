/* Copyright (C) 2007 Pierre Marchand
 *
 * Copyright (C) 2018-2019 Marco Scarpetta
 *
 * This file is part of PDF Mix Tool.
 *
 * PDF Mix Tool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PDF Mix Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PDF Mix Tool. If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <istream>
#include <ostream>
#include <iostream>
#include <cstdlib>
#include <cerrno>

#include "podofo_pdftranslator.h"

using std::ostringstream;
using std::map;
using std::vector;
using std::string;
using std::ifstream;
using std::istream;
using std::ostream;
using std::endl;
using std::runtime_error;

PdfTranslator::PdfTranslator( )
{
    std::cerr << "PdfTranslator::PdfTranslator" <<std::endl;
    sourceDoc = NULL;
    targetDoc = NULL;
    duplicate = 0;
    extraSpace = 0;
    scaleFactor = 1.0;
    pcount = 0;
    m_source_rotation = 0;
    m_source_width = 0.0;
    m_source_height = 0.0;
    destWidth = 0.0;
    destHeight = 0.0;
}

void PdfTranslator::setSource(PdfMemDocument *source)
{
    sourceDoc = source;
    pcount = sourceDoc->GetPageCount();

    // only here to avoid possible segfault, but PDF without page is not conform IIRC
    if (pcount > 0)
    {
        m_source_rotation = sourceDoc->GetPage(0)->GetRotation();

        PoDoFo::PdfRect rect (sourceDoc->GetPage(0)->GetMediaBox());
        // keep in mind it’s just a hint since PDF can have different page
        // sizes in a same doc
        m_source_width = rect.GetWidth() - rect.GetLeft();
        m_source_height = rect.GetHeight() - rect.GetBottom();
    }

    // Create target document
    targetDoc = new PdfMemDocument;

    for (int i = 0; i < pcount; ++i)
    {
        PdfPage * page = sourceDoc->GetPage(i);
        PdfMemoryOutputStream outMemStream(1);

        PdfXObject *xobj = new PdfXObject (page->GetMediaBox(), targetDoc);
        if (page->GetContents()->HasStream())
        {
            page->GetContents()->GetStream()->GetFilteredCopy(&outMemStream);
        }
        else if (page->GetContents()->IsArray())
        {
            PdfArray carray(page->GetContents()->GetArray());
            for ( unsigned int ci = 0; ci < carray.GetSize(); ++ci )
            {
                if ( carray[ci].HasStream() )
                {
                    carray[ci].GetStream()->GetFilteredCopy ( &outMemStream );
                }
                else if ( carray[ci].IsReference() )
                {
                    PdfObject *co = sourceDoc->GetObjects().GetObject(
                                carray[ci].GetReference());

                    while ( co != NULL )
                    {
                        if ( co->IsReference() )
                        {
                            co = sourceDoc->GetObjects().GetObject ( co->GetReference() );
                        }
                        else if ( co->HasStream() )
                        {
                            co->GetStream()->GetFilteredCopy ( &outMemStream );
                            break;
                        }
                    }

                }

            }
        }

        /// Its time to manage other keys of the page dictionary.
        std::vector<std::string> pageKeys;
        std::vector<std::string>::const_iterator itKey;
        pageKeys.push_back ( "Group" );
        for ( itKey = pageKeys.begin(); itKey != pageKeys.end(); ++itKey )
        {
            PoDoFo::PdfName keyname ( *itKey );
            if ( page->GetObject()->GetDictionary().HasKey ( keyname ) )
            {
                xobj->GetObject()->GetDictionary().AddKey(
                            keyname,
                            migrateResource(
                                page->GetObject()->GetDictionary().GetKey(keyname))
                            );
            }
        }

        outMemStream.Close();

        PdfMemoryInputStream inStream ( outMemStream.TakeBuffer(),outMemStream.GetLength() );
        xobj->GetContents()->GetStream()->Set ( &inStream );

        resources[i] = getInheritedResources ( page );
        xobjects[i] = xobj;
        cropRect[i] = page->GetCropBox();
        bleedRect[i] = page->GetBleedBox();
        trimRect[i] = page->GetTrimBox();
        artRect[i] = page->GetArtBox();

    }


    targetDoc->SetPdfVersion ( sourceDoc->GetPdfVersion() );

    PdfInfo *sInfo ( sourceDoc->GetInfo() );
    PdfInfo *tInfo ( targetDoc->GetInfo() );

    if ( sInfo->GetAuthor() != PdfString::StringNull )
        tInfo->SetAuthor ( sInfo->GetAuthor() );
    if ( sInfo->GetCreator() != PdfString::StringNull )
        tInfo->SetCreator ( sInfo->GetCreator() );
    if ( sInfo->GetSubject() != PdfString::StringNull )
        tInfo->SetSubject ( sInfo->GetSubject() );
    if ( sInfo->GetTitle() != PdfString::StringNull )
        tInfo->SetTitle ( sInfo->GetTitle() );
    if ( sInfo->GetKeywords() != PdfString::StringNull )
        tInfo->SetKeywords ( sInfo->GetKeywords() );

    if ( sInfo->GetTrapped() != PdfName::KeyNull )
        tInfo->SetTrapped ( sInfo->GetTrapped() );


    // 	PdfObject *scat( sourceDoc->GetCatalog() );
    // 	PdfObject *tcat( targetDoc->GetCatalog() );
    // 	TKeyMap catmap = scat->GetDictionary().GetKeys();
    // 	for ( TCIKeyMap itc = catmap.begin(); itc != catmap.end(); ++itc )
    // 	{
    // 		if(tcat->GetDictionary().GetKey(itc->first) == 0)
    // 		{
    // 			PdfObject *o = itc->second;
    // 			tcat->GetDictionary().AddKey (itc->first , migrateResource( o ) );
    // 		}
    // 	}

    // 	delete sourceDoc;
}

PdfObject* PdfTranslator::migrateResource ( PdfObject * obj )
{
    // 			std::cerr<<"PdfTranslator::migrateResource"<<std::endl;
    PdfObject *ret ( 0 );

    if ( obj->IsDictionary() )
    {
        ret = targetDoc->GetObjects().CreateObject ( *obj );

        TKeyMap resmap = obj->GetDictionary().GetKeys();
        for ( TCIKeyMap itres = resmap.begin(); itres != resmap.end(); ++itres )
        {
            PdfObject *o = itres->second;
            ret->GetDictionary().AddKey ( itres->first , migrateResource ( o ) );
        }

        if ( obj->HasStream() )
        {
            * ( ret->GetStream() ) = * ( obj->GetStream() );
        }
    }
    else if ( obj->IsArray() )
    {
        PdfArray carray ( obj->GetArray() );
        PdfArray narray;
        for ( unsigned int ci = 0; ci < carray.GetSize(); ++ci )
        {
            PdfObject *co ( migrateResource ( &carray[ci] ) );
            narray.push_back ( *co );
        }
        ret = targetDoc->GetObjects().CreateObject ( narray );
    }
    else if ( obj->IsReference() )
    {
        if ( migrateMap.find ( obj->GetReference().ToString() ) != migrateMap.end() )
        {
            return migrateMap[obj->GetReference().ToString() ];
        }

        PdfObject *o(
                    migrateResource(
                        sourceDoc->GetObjects().GetObject (obj->GetReference()))
                    );

        ret  = new PdfObject ( o->Reference() ) ;

    }
    else
    {
        ret = new PdfObject ( *obj );//targetDoc->GetObjects().CreateObject(*obj);
    }


    migrateMap.insert (
                std::pair<std::string, PdfObject*> ( obj->Reference().ToString(), ret )
                );


    return ret;

}

PdfObject* PdfTranslator::getInheritedResources ( PdfPage* page )
{
    // 			std::cerr<<"PdfTranslator::getInheritedResources"<<std::endl;
    PdfObject *res ( 0 ); // = new PdfObject;
    PdfObject *rparent = page->GetObject();
    while ( rparent && rparent->IsDictionary() )
    {
        PdfObject *curRes = rparent->GetDictionary().GetKey ( PdfName ( "Resources" ) );
        if ( curRes )
        {
            res = migrateResource ( curRes );
        }
        rparent = rparent->GetIndirectKey ( "Parent" );
    }
    return res;

}

PdfMemDocument *PdfTranslator::impose(const Multipage &multipage)
{
    // 			std::cerr<<"PdfTranslator::impose"<<std::endl;
    if ( !targetDoc )
        throw std::invalid_argument ( "impose() called with empty target" );

    // Pages size and orientation settings
    int plate_page_count = multipage.rows * multipage.columns;

    double dest_width, dest_height, margin_top, margin_bottom, margin_left, margin_right;
    int rows, columns;
    double spacing = multipage.spacing * cm;

    if (
            (m_source_rotation == 0 && multipage.rotation == 0) ||
            (m_source_rotation == 270 && multipage.rotation == 90)
            )
    {
        dest_width = multipage.page_width * cm;
        dest_height = multipage.page_height * cm;
        margin_top = multipage.margin_top * cm;
        margin_bottom = multipage.margin_bottom * cm;
        margin_left = multipage.margin_left * cm;
        margin_right = multipage.margin_right * cm;
        rows = multipage.rows;
        columns = multipage.columns;
    }
    else if (
             (m_source_rotation == 90 && multipage.rotation == 0) ||
             (m_source_rotation == 0 && multipage.rotation == 90)
             )
    {
        dest_width = multipage.page_height * cm;
        dest_height = multipage.page_width * cm;
        margin_top = multipage.margin_left * cm;
        margin_bottom = multipage.margin_right * cm;
        margin_left = multipage.margin_top * cm;
        margin_right = multipage.margin_bottom * cm;
        rows = multipage.columns;
        columns = multipage.rows;
    }
    else if (
             (m_source_rotation == 180 && multipage.rotation == 0) ||
             (m_source_rotation == 90 && multipage.rotation == 90)
             )
    {
        dest_width = multipage.page_width * cm;
        dest_height = multipage.page_height * cm;
        margin_top = multipage.margin_bottom * cm;
        margin_bottom = multipage.margin_top * cm;
        margin_left = multipage.margin_right * cm;
        margin_right = multipage.margin_left * cm;
        rows = multipage.rows;
        columns = multipage.columns;
    }
    else if (
             (m_source_rotation == 270 && multipage.rotation == 0) ||
             (m_source_rotation == 180 && multipage.rotation == 90)
             )
    {
        dest_width = multipage.page_height * cm;
        dest_height = multipage.page_width * cm;
        margin_top = multipage.margin_right * cm;
        margin_bottom = multipage.margin_left * cm;
        margin_left = multipage.margin_bottom * cm;
        margin_right = multipage.margin_top * cm;
        rows = multipage.columns;
        columns = multipage.rows;
    }

    double available_width = (
                dest_width - margin_left - margin_right - spacing * (columns - 1)
                ) / columns;

    double available_height = (
                dest_height - margin_top - margin_bottom - spacing * (rows - 1)
                ) / rows;

    scaleFactor = std::min(
                available_width / m_source_width,
                available_height / m_source_height
                );

    double page_width = m_source_width * scaleFactor;
    double page_height = m_source_height * scaleFactor;

    // Alignment settings
    double delta_x = 0, delta_y = 0;

    if (
            (m_source_rotation == 0 && multipage.rotation == 0) ||
            (m_source_rotation == 270 && multipage.rotation == 90)
            )
    {
        if (multipage.h_alignment == Multipage::Center)
            delta_x = (available_width - page_width) / 2;
        else if (multipage.h_alignment == Multipage::Right)
            delta_x = available_width - page_width;

        if (multipage.v_alignment == Multipage::Center)
            delta_y = (available_height - page_height) / 2;
        else if (multipage.v_alignment == Multipage::Top)
            delta_y = available_height - page_height;
    }
    else if (
             (m_source_rotation == 90 && multipage.rotation == 0) ||
             (m_source_rotation == 0 && multipage.rotation == 90)
             )
    {
        if (multipage.v_alignment == Multipage::Center)
            delta_x = (available_width - page_width) / 2;
        else if (multipage.v_alignment == Multipage::Bottom)
            delta_x = available_width - page_width;

        if (multipage.h_alignment == Multipage::Center)
            delta_y = (available_height - page_height) / 2;
        else if (multipage.h_alignment == Multipage::Right)
            delta_y = available_height - page_height;
    }
    else if (
             (m_source_rotation == 180 && multipage.rotation == 0) ||
             (m_source_rotation == 90 && multipage.rotation == 90)
             )
    {
        if (multipage.h_alignment == Multipage::Center)
            delta_x = (available_width - page_width) / 2;
        else if (multipage.h_alignment == Multipage::Left)
            delta_x = available_width - page_width;

        if (multipage.v_alignment == Multipage::Center)
            delta_y = (available_height - page_height) / 2;
        else if (multipage.v_alignment == Multipage::Bottom)
            delta_y = available_height - page_height;
    }
    else if (
             (m_source_rotation == 270 && multipage.rotation == 0) ||
             (m_source_rotation == 180 && multipage.rotation == 90)
             )
    {
        if (multipage.v_alignment == Multipage::Center)
            delta_x = (available_width - page_width) / 2;
        else if (multipage.v_alignment == Multipage::Top)
            delta_x = available_width - page_width;

        if (multipage.h_alignment == Multipage::Center)
            delta_y = (available_height - page_height) / 2;
        else if (multipage.h_alignment == Multipage::Left)
            delta_y = available_height - page_height;
    }

    //			PdfObject trimbox;
    //			PdfRect trim ( 0, 0, destWidth, destHeight );
    //			trim.ToVariant ( trimbox );
    std::map<int, PdfRect> *bbIndex = NULL;
    if(boundingBox.size() > 0)
    {
        if(boundingBox.find("crop") != std::string::npos)
        {
            bbIndex = &cropRect;
        }
        else if(boundingBox.find("bleed") != std::string::npos)
        {
            bbIndex = &bleedRect;
        }
        else if(boundingBox.find("trim") != std::string::npos)
        {
            bbIndex = &trimRect;
        }
        else if(boundingBox.find("art") != std::string::npos)
        {
            bbIndex = &artRect;
        }
    }

    int current_page = 0;

    while (current_page < pcount)
    {
        PdfPage *newpage = targetDoc->CreatePage(
                    PdfRect (0.0, 0.0, dest_width, dest_height)
                    );

        // 		newpage->GetObject()->GetDictionary().AddKey ( PdfName ( "TrimBox" ), trimbox );

        PdfDictionary xdict;

        ostringstream buffer;

        for (int i = 0; i < plate_page_count; i++)
        {
            // 					std::cerr<<curRecord.sourcePage<< " " << curRecord.destPage<<std::endl;
            if(current_page < pcount)
            {
                int matrix_row, matrix_col;

                if (m_source_rotation == 0)
                {
                    matrix_row = i / columns;
                    matrix_col = i % columns;
                }
                else if (m_source_rotation == 90)
                {
                    matrix_row = rows - 1 - (i % rows);
                    matrix_col = i / rows;
                }
                else if (m_source_rotation == 180)
                {
                    matrix_row = rows - 1 - (i / columns);
                    matrix_col = columns - 1 - (i % columns);
                }
                else if (m_source_rotation == 270)
                {
                    matrix_row = i % rows;
                    matrix_col = columns - 1 - (i / rows);
                }

                double tx = margin_left + (available_width + spacing) * matrix_col + delta_x;
                double ty = dest_height - margin_top - (available_height + spacing) * matrix_row -
                        available_height + delta_y;

                /*(curRecord.duplicateOf > 0) ? curRecord.duplicateOf : */
                int resourceIndex(current_page);
                PdfXObject *xo = xobjects[resourceIndex];
                if(NULL != bbIndex)
                {
                    PdfObject bb;
                    // DominikS: Fix compilation using Visual Studio on Windows
                    // bbIndex->at(resourceIndex).ToVariant( bb );
                    ((*bbIndex)[resourceIndex]).ToVariant( bb );
                    xo->GetObject()->GetDictionary().AddKey ( PdfName ( "BBox" ), bb );
                }
                ostringstream op;
                op << "OriginalPage" << resourceIndex;
                xdict.AddKey ( PdfName ( op.str() ) , xo->GetObjectReference() );

                if ( resources[resourceIndex] )
                {
                    if ( resources[resourceIndex]->IsDictionary() )
                    {
                        TKeyMap resmap = resources[resourceIndex]->GetDictionary().GetKeys();
                        TCIKeyMap itres;
                        for ( itres = resmap.begin(); itres != resmap.end(); ++itres )
                        {
                            xo->GetResources()->GetDictionary().AddKey(
                                        itres->first, itres->second
                                        );
                        }
                    }
                    else if ( resources[resourceIndex]->IsReference() )
                    {
                        xo->GetObject()->GetDictionary().AddKey(
                                    PdfName ( "Resources" ), resources[resourceIndex]
                                    );
                    }
                    else
                        std::cerr << "ERROR Unknown type resource " <<
                                     resources[resourceIndex]->GetDataTypeString() <<
                                     std::endl;

                }
                // Very primitive but it makes it easy to track down imposition
                // plan into content stream.
                buffer << "q\n";
                buffer << std::fixed <<
                          // Translation
                          "1 0 0 1 " << tx << " " <<  ty << " cm\n" <<
                          // Rotation (clockwise)
                          // cosR << " " << -sinR << " " << sinR << " " << cosR << " 0 0 cm\n" <<
                          // Scale
                          scaleFactor << " 0 0 " << scaleFactor << " 0 0 cm\n";
                buffer << "/OriginalPage" << resourceIndex << " Do\n";
                buffer << "Q\n";
            }

            ++current_page;
        }
        if (!newpage)
            PODOFO_RAISE_ERROR (ePdfError_ValueOutOfRange);
        string bufStr = buffer.str();
        newpage->GetContentsForAppending()->GetStream()->Set ( bufStr.data(), bufStr.size() );
        newpage->GetResources()->GetDictionary().AddKey ( PdfName ( "XObject" ), xdict );
    }

    return targetDoc;
}
