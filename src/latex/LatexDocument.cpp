/* Copyright 2017 The sfcpp Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/



#include "LatexDocument.hpp"
#include <boost/filesystem.hpp>
#include <files/files.hpp>

#include <cstdlib>

namespace sfcpp {
namespace latex {

extern std::string documentHeader;

std::string LatexDocument::getCode() const {
  return "\\documentclass[tikz,border=0.1cm]{standalone}\n" + documentHeader +
         "\n\\begin{document}\n" + container.getCode() + "\\end{document}\n";  // TODO
}

void LatexDocument::addElement(std::shared_ptr<LatexElement> element) {
  container.addElement(element);
}

void LatexDocument::saveAndCompile(std::string filename) {
  sfcpp::files::writeToFile(filename, getCode());

  std::string command;

  boost::filesystem::path p(filename);
  auto parentPath = p.parent_path();
  if (!parentPath.empty()) {
    command += "cd " + parentPath.native() + "; ";
  }

  command += "pdflatex -interaction=batchmode " + p.filename().native();
  system(command.c_str());
}

std::string documentHeader = R"package(
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{lmodern}
\usepackage[ngerman]{babel}
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{amsthm}
\usepackage{mathtools}
\usepackage{IEEEtrantools}
\usepackage{microtype}
\usepackage{amsopn}
\usepackage{varwidth} 
\usepackage{xcolor,colortbl}
\usepackage{tabu}
\usepackage{comment}
\usepackage{enumerate}
\usepackage{array}
\usepackage{csquotes}
\usepackage{tikz}
\usepackage{tikz-qtree}
\usepackage{tikz-3dplot}
\usepackage{nicefrac}
\usetikzlibrary{trees,positioning,shapes,calc,patterns,angles,quotes}
%\usepackage[squaren]{SIunits}
\usepackage{pgfplots}
\usepackage{algpseudocode}
\usepackage{algorithm}
%\usepackage{hyperref}
\usepackage{booktabs}
\usepackage{amsfonts}
\usepackage{color}
\usepackage{stmaryrd}
\usepackage[shortlabels]{enumitem}
\usepackage{todonotes}
\usepackage{mathdots}
\usepackage{empheq}
\usepgfplotslibrary{fillbetween}

\pgfplotsset{compat=1.11}
\usepackage[caption=false]{subfig}

\usepackage{etoolbox}

\makeatletter
\providerobustcmd*{\bigcupdot}{%
  \mathop{%
    \mathpalette\bigop@dot\bigcup
  }%
}
\newrobustcmd*{\bigop@dot}[2]{%
  \setbox0=\hbox{$\m@th#1#2$}%
  \vbox{%
    \lineskiplimit=\maxdimen
    \lineskip=-0.7\dimexpr\ht0+\dp0\relax
    \ialign{%
      \hfil##\hfil\cr
      $\m@th\cdot$\cr
      \box0\cr
    }%
  }%
}
\makeatother

\newcommand*\widefbox[1]{\fbox{\hspace{2em}#1\hspace{2em}}}

\newlength\Origarrayrulewidth

% horizontal rule equivalent to \cline but with 2pt width
\newcommand{\Cline}[1]{%
 \noalign{\global\setlength\Origarrayrulewidth{\arrayrulewidth}}%
 \noalign{\global\setlength\arrayrulewidth{2pt}}\cline{#1}%
 \noalign{\global\setlength\arrayrulewidth{\Origarrayrulewidth}}%
}

% draw a vertical rule of width 2pt on both sides of a cell
\newcommand\Thickvrule[1]{%
  \multicolumn{1}{!{\vrule width 2pt}c!{\vrule width 2pt}}{#1}%
}

% draw a vertical rule of width 2pt on the left side of a cell
\newcommand\Thickvrulel[1]{%
  \multicolumn{1}{!{\vrule width 2pt}c|}{#1}%
}

% draw a vertical rule of width 2pt on the right side of a cell
\newcommand\Thickvruler[1]{%
  \multicolumn{1}{|c!{\vrule width 2pt}}{#1}%
}

\theoremstyle{plain}
\newtheorem{theorem}{Satz}
\newtheorem{satz}{Satz}
\newtheorem{lemma}[theorem]{Lemma}
\newtheorem{proposition}[theorem]{Proposition}
\newtheorem{Theorem}[theorem]{Theorem}
\newtheorem{corollary}[theorem]{Korollar}

\theoremstyle{definition}
\newtheorem{defi}[theorem]{Definition}
\newtheorem{definition}[theorem]{Definition}
\newtheorem{algorithmThm}[theorem]{Algorithmus}
\newtheorem{remark}[theorem]{Bemerkung}
\newtheorem{bem}[theorem]{Bemerkung}
\newtheorem{example}[theorem]{Beispiel}
\newtheorem{bsp}[theorem]{Beispiel}
\newtheorem{outlook}[theorem]{Ausblick}
\newtheorem{notation}[theorem]{Notation}

%\newenvironment{bsp}{\par\noindent\textbf{\thechapter. Beispiel}}{}

\newcommand{\myworries}[1]{\textcolor{red}{#1}}

\newtheorem{exercise}{Aufgabe}

\numberwithin{exercise}{section}
\numberwithin{equation}{section}
\numberwithin{theorem}{section}

\newcommand{\bbA}{\mathbb{A}}
\newcommand{\bbB}{\mathbb{B}}
\newcommand{\bbC}{\mathbb{C}}
\newcommand{\bbD}{\mathbb{D}}
\newcommand{\bbE}{\mathbb{E}}
\newcommand{\bbF}{\mathbb{F}}
\newcommand{\bbG}{\mathbb{G}}
\newcommand{\bbH}{\mathbb{H}}
\newcommand{\bbI}{\mathbb{I}}
\newcommand{\bbJ}{\mathbb{J}}
\newcommand{\bbK}{\mathbb{K}}
\newcommand{\bbL}{\mathbb{L}}
\newcommand{\bbM}{\mathbb{M}}
\newcommand{\bbN}{\mathbb{N}}
\newcommand{\bbO}{\mathbb{O}}
\newcommand{\bbP}{\mathbb{P}}
\newcommand{\bbQ}{\mathbb{Q}}
\newcommand{\bbR}{\mathbb{R}}
\newcommand{\bbS}{\mathbb{S}}
\newcommand{\bbT}{\mathbb{T}}
\newcommand{\bbU}{\mathbb{U}}
\newcommand{\bbV}{\mathbb{V}}
\newcommand{\bbW}{\mathbb{W}}
\newcommand{\bbX}{\mathbb{X}}
\newcommand{\bbY}{\mathbb{Y}}
\newcommand{\bbZ}{\mathbb{Z}}

\newcommand{\calA}{\mathcal{A}}
\newcommand{\calB}{\mathcal{B}}
\newcommand{\calC}{\mathcal{C}}
\newcommand{\calD}{\mathcal{D}}
\newcommand{\calE}{\mathcal{E}}
\newcommand{\calF}{\mathcal{F}}
\newcommand{\calG}{\mathcal{G}}
\newcommand{\calH}{\mathcal{H}}
\newcommand{\calI}{\mathcal{I}}
\newcommand{\calJ}{\mathcal{J}}
\newcommand{\calK}{\mathcal{K}}
\newcommand{\calL}{\mathcal{L}}
\newcommand{\calM}{\mathcal{M}}
\newcommand{\calN}{\mathcal{N}}
\newcommand{\calO}{\mathcal{O}}
\newcommand{\calP}{\mathcal{P}}
\newcommand{\calQ}{\mathcal{Q}}
\newcommand{\calR}{\mathcal{R}}
\newcommand{\calS}{\mathcal{S}}
\newcommand{\calT}{\mathcal{T}}
\newcommand{\calU}{\mathcal{U}}
\newcommand{\calV}{\mathcal{V}}
\newcommand{\calW}{\mathcal{W}}
\newcommand{\calX}{\mathcal{X}}
\newcommand{\calY}{\mathcal{Y}}
\newcommand{\calZ}{\mathcal{Z}}

%Miscellaneous
\newcommand{\diff}{d} %{\,\mathrm{d}}
\newcommand{\ddx}{\frac{\diff}{\diff x}}
\newcommand{\quot}[1]{\enquote{#1}}
\newcommand{\setof}[1]{\widehat{#1}} %die Menge einer Algebra
\newcommand{\WLOG}{without loss of generality}
\DeclareMathOperator{\fst}{fst}
\DeclareMathOperator{\snd}{snd}

%Functions
\newcommand{\successor}[1]{s(#1)}
\newcommand{\predecessor}[1]{p(#1)}
\newcommand{\successors}[1]{S_{#1}}
\newcommand{\predecessors}[1]{P_{#1}}
\newcommand{\innerSet}[1]{\widehat{#1}}

%Operators for Relations
\newcommand{\equalDef}{\coloneqq}
\newcommand{\defEqual}{\eqqcolon}
\newcommand{\equivDef}{:\Leftrightarrow}

%Operators for Functions
\newcommand{\dProd}[2]{\left\langle #1, #2 \right\rangle}
\newcommand{\revdProd}[2]{\,\overleftrightarrow{\!\dProd{#1}{#2}\!}\,}
\newcommand{\revrevdProd}[2]{\,\overleftrightarrow{\overleftrightarrow{\!\dProd{#1}{#2}\!}}\,}
\newcommand{\dProdFunc}{\dProd{\cdot}{\cdot}}
\newcommand{\revdProdFunc}{\revdProd{\cdot}{\cdot}}
\newcommand{\revrevdProdFunc}{\revrevdProd{\cdot}{\cdot}}
\newcommand{\norm}[1]{\| #1 \|}
\newcommand{\logtwo}{\log_2}

%Operators for Sets
\newcommand{\powerset}[1]{\calP(#1)}
\newcommand{\card}[1]{|#1|}

\DeclareMathOperator{\Hom}{Hom}
\DeclareMathOperator{\Kern}{Kern}
\DeclareMathOperator{\Bild}{Bild}
\DeclareMathOperator{\im}{im}
\DeclareMathOperator{\id}{id}
\DeclareMathOperator{\cod}{cod}
\DeclareMathOperator{\dom}{dom}
\DeclareMathOperator*{\argmax}{argmax}

%Concrete Set Identifiers
\newcommand{\steadyOn}[1]{C(#1)}

\newcommand{\natOne}{\bbZ^+}
\newcommand{\natZero}{\bbN_0}
\newcommand{\oneToN}[1]{\bbN^{(#1)}}
\newcommand{\integers}{\bbZ}
\newcommand{\rationals}{\bbQ}
\newcommand{\reals}{\bbR}
\newcommand{\complex}{\bbC}

\newcommand{\posReals}{\bbR^+}
\newcommand{\nonNegReals}{\bbR_0^+}
\newcommand{\negReals}{\bbR^-}
\newcommand{\nonPosReals}{\bbR_0^-}

\newcommand{\domain}[1]{\dom \left( #1 \right)}
\newcommand{\codomain}[1]{\cod \left( #1 \right)}
\newcommand{\image}[1]{\im \left( #1 \right)}

\DeclareMathOperator{\rePart}{Re}
\DeclareMathOperator{\imPart}{Im}

%Complexity Sets
%\newcommand{\compEq}[2][]{\Theta_{#1} \left( #2 \right)}
%\newcommand{\compLess}[2][]{o_{#1} \left( #2 \right)}
%\newcommand{\compLeq}[2][]{\calO_{#1} \left( #2 \right)}
%\newcommand{\compGreater}[2][]{\omega_{#1} \left( #2 \right)}
%\newcommand{\compGeq}[2][]{\Omega_{#1} \left( #2 \right)}
\newcommand{\compEq}[1]{\Theta \left( #1 \right)}
\newcommand{\compLess}[1]{o\left( #1 \right)}
\newcommand{\compLeq}[1]{O\left( #1 \right)}
\newcommand{\compGreater}[1]{\omega\left( #1 \right)}
\newcommand{\compGeq}[1]{\Omega\left( #1 \right)}
\newcommand{\subsEq}{\subseteq}
\newcommand{\subsNeq}{\subsetneq}

\newcommand{\assign}{\coloneqq}

\newcommand{\mybracketl}[1]{[}
\newcommand{\mybracketr}[1]{]}

\providecommand{\piValue}{3.1415926535897932384626433832795028841}

\pgfplotscreateplotcyclelist{custom black white}{%
solid, every mark/.append style={solid, fill=gray}, mark=*\\%
dotted, every mark/.append style={solid, fill=gray}, mark=square*\\%
densely dotted, every mark/.append style={solid, fill=gray}, mark=triangle*\\%
dashdotted, every mark/.append style={solid, fill=gray}, mark=star\\%
dashed, every mark/.append style={solid, fill=gray},mark=diamond*\\%
loosely dashed, every mark/.append style={solid, fill=gray},mark=*\\%
densely dashed, every mark/.append style={solid, fill=gray},mark=square*\\%
dashdotted, every mark/.append style={solid, fill=gray},mark=otimes*\\%
dashdotdotted, every mark/.append style={solid},mark=star\\%
densely dashdotted,every mark/.append style={solid, fill=gray},mark=diamond*\\%
}
)package";

} /* namespace latex */
} /* namespace utils */
